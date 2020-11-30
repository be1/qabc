#include "EditVBoxLayout.h"
#include "AbcApplication.h"
#include "PreferencesMenu.h"
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QSpinBox>
#include <QDir>
#include <QStandardPaths>
#ifdef USE_LIBABCM2PS
#include "../abcm2ps/abcm2ps.h"
#endif

EditVBoxLayout::EditVBoxLayout(const QString& fileName, QWidget* parent)
	: QVBoxLayout(parent),
	abcplaintextedit(parent),
	playpushbutton(parent),
	runpushbutton(parent),
	hboxlayout(parent),
	xspinbox(parent),
	xlabel(parent),
    fileName(fileName),
    waiter(nullptr),
	id(nullptr),
	drv(nullptr),
	sf(nullptr),
	mf(nullptr)
{
    setObjectName("EditVBoxLayout:" + fileName);
    QString t = QDir::tempPath() + QDir::separator() + "qabc-XXXXXX.abc";
	tempFile.setFileTemplate(t);
    xspinbox.setMinimum(1);
	xlabel.setText(tr("X:"));
	xlabel.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	xlabel.setBuddy(&xspinbox);
	hboxlayout.addWidget(&xlabel);
	hboxlayout.addWidget(&xspinbox);
	hboxlayout.addWidget(&playpushbutton);
	hboxlayout.addWidget(&runpushbutton);

	addWidget(&abcplaintextedit);
	addLayout(&hboxlayout);

	connect(&xspinbox, SIGNAL(valueChanged(int)), this, SLOT(onXChanged(int)));
	connect(&playpushbutton, SIGNAL(clicked()), this, SLOT(onPlayClicked()));
	connect(&runpushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onRunClicked);

	connect(this, &EditVBoxLayout::playerFinished, this, &EditVBoxLayout::onPlayFinished);
    connect(this, &EditVBoxLayout::compilerFinished, this, &EditVBoxLayout::onCompileFinished);

    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QByteArray ba;
    ba = settings.value(DRIVER_KEY).toString().toUtf8();
    drv = (char*) realloc(drv, ba.length() + 1);
    strncpy(drv, ba.constData(), ba.length());
    drv[ba.length()] = '\0';

    qDebug() << fileName << drv;
    fluid_settings = new_fluid_settings();
    fluid_settings_setstr(fluid_settings, "audio.driver", drv);
    fluid_settings_setint(fluid_settings, "audio.jack.autoconnect", 1);

    QFileInfo info(fileName);
    QString name = "qabc-" + info.baseName();
    ba = name.toUtf8();
    id = (char*) realloc(id, ba.length() + 1);
    strncpy(id, ba.constData(), ba.length());
    id[ba.length()] = '\0';
    fluid_settings_setstr(fluid_settings, "audio.jack.id", id);

    qDebug() << id << drv;

    fluid_synth = new_fluid_synth(fluid_settings);
    fluid_adriver = new_fluid_audio_driver(fluid_settings, fluid_synth);
}


EditVBoxLayout::~EditVBoxLayout()
{
    /* stoppping synthesis */
    if (waiter && waiter->isRunning())
        waiter->terminate();
    if (fluid_player) {
        fluid_player_stop(fluid_player);
        fluid_player_join(fluid_player);
        delete_fluid_player(fluid_player);
        fluid_player = nullptr;
    }
    delete_fluid_audio_driver(fluid_adriver);
    delete_fluid_synth(fluid_synth);
    delete_fluid_settings(fluid_settings);
    free(id);
    free(drv);
    free(sf);
	free(mf);

    /* cleanup files manually */
    qDebug() << "cleaning MIDI for" << tempFile.fileName();
    QString temp(tempFile.fileName());
    temp.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid");
    if (QFileInfo::exists(temp))
        QFile::remove(temp);

    qDebug() << "cleaning SVG for" << tempFile.fileName();
    for (int i = 1; i < 999; i++) {
        temp = tempFile.fileName();
        temp.replace(QRegularExpression("\\.abc$"), QString::asprintf("%03d.svg", i));
        if (!QFileInfo::exists(temp))
            break;
        QFile::remove(temp);
    }
}

AbcPlainTextEdit *EditVBoxLayout::abcPlainTextEdit()
{
	return &abcplaintextedit;
}

PlayPushButton *EditVBoxLayout::playPushButton()
{
	return &playpushbutton;
}

RunPushButton *EditVBoxLayout::runPushButton()
{
	return &runpushbutton;
}

void EditVBoxLayout::setFileName(const QString &fn)
{
    fileName = fn;
}

void EditVBoxLayout::cleanupProcesses()
{
    /* early kill in case  we quit the app brutally */
    for (int i = 0; i < processlist.length(); i++)
        processlist.at(i)->kill();
}

void EditVBoxLayout::onXChanged(int value)
{
    qDebug() << value;
}

void EditVBoxLayout::spawnCompiler(const QString &prog, const QStringList& args, const QDir &wrk)
{
	AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    w->mainHSplitter()->viewWidget()->logView()->clear();
	return spawnProgram(prog, args, AbcProcess::ProcessCompiler, wrk);
}

void EditVBoxLayout::spawnPlayer(const QString& prog, const QStringList &args, const QDir &wrk)
{
	AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow *w = a->mainWindow();
    w->mainHSplitter()->viewWidget()->logView()->clear();
	return spawnProgram(prog, args, AbcProcess::ProcessPlayer, wrk);
}

void EditVBoxLayout::spawnProgram(const QString& prog, const QStringList& args, AbcProcess::ProcessType which, const QDir& wrk)
{
	AbcProcess *process = new AbcProcess(which, this);
    process->setWorkingDirectory(wrk.absolutePath());
	connect(process, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType>::of(&AbcProcess::finished), this, &EditVBoxLayout::onProgramFinished);
    connect(process, &AbcProcess::outputText, this, &EditVBoxLayout::onProgramOutputText);
    connect(process, &AbcProcess::errorText, this, &EditVBoxLayout::onProgramErrorText);
    processlist.append(process);
	qDebug() << prog << args;
	process->start(prog, args);
}

void EditVBoxLayout::onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType which)
{
    switch (which) {
    case AbcProcess::ProcessPlayer:
        emit playerFinished(exitCode); break;
    case AbcProcess::ProcessCompiler:
        emit compilerFinished(exitCode); break;
    case AbcProcess::ProcessUnknown:
    default:
        break;
    }

	/* delete garbage */
	for (int i = 0; i < processlist.length(); i++) {
		AbcProcess* proc = processlist.at(i);
		if (proc->state() == QProcess::NotRunning
				&& proc->exitCode() == exitCode
				&& proc->exitStatus() == exitStatus
				&& proc->which() == which) {
			disconnect(proc, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType>::of(&AbcProcess::finished), this, &EditVBoxLayout::onProgramFinished);
			delete proc;
			processlist.removeAt(i);
		}
    }
}

void EditVBoxLayout::onProgramOutputText(const QByteArray &text)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    LogView* lv = w->mainHSplitter()->viewWidget()->logView();
    lv->appendHtml("<em>" + QString::fromUtf8(text).replace("\n", "<br />") + "</em>");
}

void EditVBoxLayout::onProgramErrorText(const QByteArray &text)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    LogView* lv = w->mainHSplitter()->viewWidget()->logView();
    lv->appendHtml("<b style=\"color: red\">" + QString::fromUtf8(text).replace("\n", "<br />") + "</b>");
}

void EditVBoxLayout::onPlayClicked()
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    if (playpushbutton.isPlay()) {
        a->mainWindow()->statusBar()->showMessage(tr("Generating MIDI for playing."));
        playpushbutton.flip();
        xspinbox.setEnabled(false);
        QString tosave = abcPlainTextEdit()->toPlainText();
        tempFile.open();
        tempFile.write(tosave.toUtf8());
        tempFile.close();
        QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
        QVariant player = settings.value(PLAYER_KEY);
        QString program = player.toString();
        QStringList argv = program.split(" ");
        program = argv.at(0);
        argv.removeAt(0);
        argv << tempFile.fileName();
        argv << QString::number(xspinbox.value());
        QFileInfo info(tempFile.fileName());
        QDir dir = info.absoluteDir();

        /* despite the name, Player is abc2midi MIDI generator */
        spawnPlayer(program, argv, dir);
    } else {
        a->mainWindow()->statusBar()->showMessage(tr("Stopping synthesis..."));
        fluid_player_stop(fluid_player);
    }
}

void EditVBoxLayout::onPlayFinished(int exitCode)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    if (exitCode) {
        a->mainWindow()->statusBar()->showMessage(tr("Error during MIDI generation."));
        playpushbutton.flip();
        xspinbox.setEnabled(true);
		return;
	}
    a->mainWindow()->statusBar()->showMessage(tr("MIDI generation finished."));

    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);

#if 0
    QVariant driver = settings.value(DRIVER_KEY);
    QByteArray ba = driver.toString().toUtf8();
    char * drv = (char*) realloc(ba.length() + 1);
    strncpy(drv, ba.constData(), ba.length());
    drv[ba.length()] = '\0';
    qDebug() << "driver:" << drv;
    fluid_settings_setstr(fluid_settings, "audio.driver", drv);
#endif


#if 0
    if (fluid_adriver)
        delete_fluid_audio_driver(fluid_adriver);
    fluid_adriver = new_fluid_audio_driver(fluid_settings, fluid_synth);
#endif
    fluid_synth_set_gain(fluid_synth, 1.0);

    if (fluid_player) {
        fluid_player_stop(fluid_player);
        fluid_player_join(fluid_player);
        delete_fluid_player(fluid_player);
    }

    QVariant soundfont = settings.value(SOUNDFONT_KEY);
    if (soundfont.toString() != curSFont) {
        curSFont = soundfont.toString();
        QByteArray ba(curSFont.toUtf8());
        sf = (char*) realloc(sf, ba.length() + 1);
        strncpy(sf, ba.constData(), ba.length());
        sf[ba.length()] = '\0';
        qDebug() << "new soundfont:" << sf;
        fluid_sfont_t* f = fluid_synth_get_sfont_by_id(fluid_synth, sfid);
        if (f)
            fluid_synth_sfunload(fluid_synth, sfid, 0);
        int fid = fluid_synth_sfload(fluid_synth, sf, 0);
        if (fid == FLUID_FAILED)
            qWarning() << "Cannot load soundfont:" << sf;
        else
            sfid = fid;
    }

    fluid_player = new_fluid_player(fluid_synth);

    if (waiter && waiter->isFinished())
            delete waiter;
    waiter = new TuneWaiter(fluid_player, this);
    connect(waiter, &TuneWaiter::playerFinished, this, &EditVBoxLayout::onSynthFinished);

	/* midi file can change from tune (xspinbox) index */
	QByteArray ba;
	QString midifile(tempFile.fileName());
	midifile.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid");
	ba = midifile.toUtf8();
	mf = (char*) realloc(mf, ba.length() + 1);
	strncpy(mf, ba.constData(), ba.length());
    mf[ba.length()] = '\0';
	qDebug() << "midifile:" << mf;

    if (FLUID_FAILED == fluid_player_add(fluid_player, mf)) {
        qWarning() << "Cannot not add MIDI file:" << mf;
    }

    fluid_player_play(fluid_player);
    waiter->start();

    switch (fluid_player_get_status(fluid_player)) {
    case FLUID_PLAYER_READY:
        a->mainWindow()->statusBar()->showMessage(tr("Starting synthesis..."));
        break;
    case FLUID_PLAYER_PLAYING:
        a->mainWindow()->statusBar()->showMessage(tr("Synthesis playing..."));
        break;
    case FLUID_PLAYER_DONE:
        a->mainWindow()->statusBar()->showMessage(tr("Synthesis done."));
        break;
    default:
        break;
    }
}

void EditVBoxLayout::onSynthFinished(int exitCode)
{
    qDebug() << "synth" << exitCode;

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    a->mainWindow()->statusBar()->showMessage(tr("Synthesis finished."));

    int x = xspinbox.value();
    QString midi (tempFile.fileName());
    midi.replace(QRegularExpression("\\.abc$"), QString::number(x) + ".mid");
    QFile::remove(midi);

    playpushbutton.flip();
    xspinbox.setEnabled(true);
}

void EditVBoxLayout::onRunClicked()
{
    runpushbutton.setEnabled(false);
    /* do not disable/enable xspinbox because Play manages it for audio rendering! */

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    a->mainWindow()->statusBar()->showMessage(tr("Generating score..."));
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
#ifndef USE_LIBABCM2PS
    QVariant compiler = settings.value(COMPILER_KEY);
    QString program = compiler.toString();
#else
	QString program("abcm2ps");
#endif
    QStringList argv = program.split(" ");
    program = argv.at(0);
#ifndef USE_LIBABCM2PS
    argv.removeAt(0);
#endif
    argv << "-v"; /* SVG output*/
    argv << "-e" << QString::number(xspinbox.value()); /* one tune to consider */
    argv << "-N1"; /* numbering pages on  the up-left */
    QString temp(tempFile.fileName());
    temp.replace(QRegularExpression("\\.abc$"), ".svg"); /* but remind, output will be tempNNN.svg */
    argv << "-O" << temp; /* output filename */
    argv << tempFile.fileName(); /* input ABC */

    QFileInfo info(tempFile.fileName());
    QDir dir = info.absoluteDir();
#ifdef USE_LIBABCM2PS
	QString s;
	QByteArray ba;
    char **av = (char**)malloc(argv.length() * sizeof (char*));
    for (int i = 0; i < argv.length(); i++) {
        s = argv.at(i);
        ba = s.toUtf8();
        av[i] = (char*)malloc((ba.length() + 1) * sizeof (char));
        strncpy(av[i], ba.constData(), ba.length());
        av[i][ba.length()] = '\0';
    }

    int ret = abcm2ps(argv.length(), av);

	for (int i = 0; i < argv.length(); i++) {
		free(av[i]);
	}
	free(av);
    emit compilerFinished(ret);
#else
    spawnCompiler(program, argv, dir);
#endif
}

void EditVBoxLayout::onCompileFinished(int exitCode)
{
    qDebug() << "compile" << exitCode;

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (exitCode < 0 || exitCode > 1) { /* sometimes, abcm2ps returns 1 even on 'success' */
        a->mainWindow()->statusBar()->showMessage(tr("Error during score generation."));
        return;
    }

    a->mainWindow()->statusBar()->showMessage(tr("Score generated."));
    QFileInfo info(tempFile);
    QString b(info.baseName());
    QString d = info.dir().absolutePath();
    a->mainWindow()->mainHSplitter()->viewWidget()->initBasename(b, d);
    a->mainWindow()->mainHSplitter()->viewWidget()->requestPage(1);

    runpushbutton.setEnabled(true);
}
