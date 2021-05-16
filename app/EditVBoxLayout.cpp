#include "config.h"
#include "EditVBoxLayout.h"
#include "AbcApplication.h"
#include "PreferencesMenu.h"
#include "sfloader.h"
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QSpinBox>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTimer>
#include "../abc/abc.h"
#include "abcsmf.h"
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
    progress(nullptr),
    synth(nullptr)
{
    setObjectName("EditVBoxLayout:" + fileName);

    QString t = QDir::tempPath() + QDir::separator() + "qabc-XXXXXX.abc";
	tempFile.setFileTemplate(t);
    xspinbox.setMinimum(1);
	xlabel.setText(tr("X:"));
	xlabel.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    xlabel.setBuddy(&xspinbox);
    progress = new QProgressIndicator();
    progress->startAnimation();

    hboxlayout.addWidget(&xlabel);
	hboxlayout.addWidget(&xspinbox);
    hboxlayout.addWidget(progress);
    hboxlayout.addWidget(&playpushbutton);
	hboxlayout.addWidget(&runpushbutton);

	addWidget(&abcplaintextedit);
	addLayout(&hboxlayout);

    connect(&abcplaintextedit, &QPlainTextEdit::selectionChanged, this, &EditVBoxLayout::onSelectionChanged);
    connect(&xspinbox, SIGNAL(valueChanged(int)), this, SLOT(onXChanged(int)));
	connect(&playpushbutton, SIGNAL(clicked()), this, SLOT(onPlayClicked()));
	connect(&runpushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onRunClicked);

    connect(this, &EditVBoxLayout::generateMIDIFinished, this, &EditVBoxLayout::onGenerateMIDIFinished);
    connect(this, &EditVBoxLayout::compilerFinished, this, &EditVBoxLayout::onCompileFinished);
    connect(this, &EditVBoxLayout::doExportMIDI, this, &EditVBoxLayout::exportMIDI);

    QFileInfo info(fileName);
    synth = new AbcSynth(info.baseName(), this);
    connect(synth, &AbcSynth::initFinished, this, &EditVBoxLayout::onSynthInited);
    connect(synth, &AbcSynth::synthFinished, this, &EditVBoxLayout::onSynthFinished);
    playpushbutton.setEnabled(false);
}


EditVBoxLayout::~EditVBoxLayout()
{
    /* stop synthesis */
    if (synth && synth->isPlaying()) {
        synth->stop();
        synth->waitFinish();
    }

    delete synth;

    removeMIDIFile();
    removeSvgFiles();
}

void EditVBoxLayout::onSynthInited(bool err) {
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    progress->stopAnimation();
    progress->deleteLater();
    progress = nullptr;
    playpushbutton.setEnabled(true);
    if (err)
        a->mainWindow()->statusBar()->showMessage(tr("Cannot load sound font."));
    else {
        a->mainWindow()->statusBar()->clearMessage();
    }
}

void EditVBoxLayout::removeMIDIFile() {
    /* cleanup files manually */
    QString temp(tempFile.fileName());
    qDebug() << "cleaning MIDI for" << temp;
    temp.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid");
    if (QFileInfo::exists(temp))
        QFile::remove(temp);
}

void EditVBoxLayout::removeSvgFiles() {
    QString temp(tempFile.fileName());
    qDebug() << "cleaning SVG for" << temp;
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
    /* early kill in case we quit the app brutally */
    for (int i = 0; i < processlist.length(); i++)
        processlist.at(i)->kill();
}

void EditVBoxLayout::cleanupThreads()
{
    if (synth->isLoading())
        synth->abort();
    else if (synth->isPlaying())
        synth->stop();
}

void EditVBoxLayout::onXChanged(int value)
{
    qDebug() << value;
}

void EditVBoxLayout::spawnSVGCompiler(const QString &prog, const QStringList& args, const QDir &wrk, int cont)
{
	AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    w->mainHSplitter()->viewWidget()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessCompiler, wrk, cont);
}

void EditVBoxLayout::spawnMIDIGenerator(const QString& prog, const QStringList &args, const QDir &wrk, int cont)
{
	AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow *w = a->mainWindow();
    w->mainHSplitter()->viewWidget()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessPlayer, wrk, cont);
}

void EditVBoxLayout::spawnProgram(const QString& prog, const QStringList& args, AbcProcess::ProcessType which, const QDir& wrk, int cont)
{
    AbcProcess *process = new AbcProcess(which, this, cont);
    process->setWorkingDirectory(wrk.absolutePath());
    connect(process, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType, int>::of(&AbcProcess::finished), this, &EditVBoxLayout::onProgramFinished);
    connect(process, &AbcProcess::outputText, this, &EditVBoxLayout::onProgramOutputText);
    connect(process, &AbcProcess::errorText, this, &EditVBoxLayout::onProgramErrorText);
    processlist.append(process);
	qDebug() << prog << args;
	process->start(prog, args);
}

void EditVBoxLayout::onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType which, int cont)
{
    switch (which) {
    case AbcProcess::ProcessPlayer:
        emit generateMIDIFinished(exitCode, cont); break;
    case AbcProcess::ProcessCompiler:
        emit compilerFinished(exitCode, cont); break;
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
            disconnect(proc, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType, int>::of(&AbcProcess::finished), this, &EditVBoxLayout::onProgramFinished);
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
    static QString dot;
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    if (playpushbutton.isPlay()) {
        a->mainWindow()->statusBar()->showMessage(tr("Generating MIDI for playing."));
        playpushbutton.flip();
        xspinbox.setEnabled(false);
        playpushbutton.setEnabled(false);
        QApplication::processEvents();
        emit doExportMIDI(QString());
      } else { /* stop */
        if (synth->isPlaying()) {
            dot.clear();
            a->mainWindow()->statusBar()->showMessage(tr("Stopping synthesis..."));
            synth->stop();
        } else if (synth->isLoading()){
            a->mainWindow()->statusBar()->showMessage(tr("Please wait...") + dot);
            dot += " ...";
        }
    }
}

void EditVBoxLayout::exportMIDI(const QString& outfilename) {
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    QString tosave;

    int virtualLineCount = 0;

    if (selection.isEmpty()) {
        tosave = abcPlainTextEdit()->toPlainText();
    } else {
        QString all = abcPlainTextEdit()->toPlainText();
        int i = 0, xl = 0;
        QStringList lines = all.split('\n');

        int l;
        /* find last X: before selectionIndex */
        for (l = 0; l < lines.count() && i < selectionIndex; l++) {
            i += lines.at(l).count() +1; /* count \n */
            if (lines.at(l).startsWith("X:")) {
                xspinbox.setValue(lines.at(l).rightRef(1).toInt());
                xl = l;
            }
        }

        int j;
        /* construct headers */
        for (j = xl;  j < lines.count(); j++) {
            if (lines.at(j).contains(QRegularExpression("^((%[^\n]*)|([A-Z]:[^\n]+))$"))) {
                if (lines.at(j).startsWith("V:")) /* ignore voice number */
                    continue;

                if (lines.at(j).startsWith("%%")) /* ignore MIDI instruction, use basic Piano */
                    continue;

                tosave += lines.at(j) + "\n";
            } else
                break;
        }

        virtualLineCount = l;
        tosave += selection;
    }

    /* early opening of tempfile to set a random name */
    /* when coming from QTextCursor::selectedText(), LF is replaced by U+2029 */
    tempFile.open();
    tempFile.write(tosave.replace(QChar::ParagraphSeparator, "\n").toUtf8());
    tempFile.close();

    int cont;
    if (outfilename.isEmpty()) {
        cont = 1; /* continue to playback */
        exportpath.clear();
    } else {
        cont = 0; /* move to exported midi file */
        exportpath = outfilename;
    }

    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant player = settings.value(PLAYER_KEY);

    if (player == LIBABC2SMF) {
        QByteArray ba = tosave.replace(QChar::ParagraphSeparator, "\n").toUtf8();
        struct abc* yy = abc_parse_buffer(ba.constData(), ba.count());

        if (yy->error) {
            int l;
            if (virtualLineCount) {
                l = virtualLineCount;
            } else {
                l = yy->error_line;
            }
            QMessageBox::warning(a->mainWindow(), tr("Error"), tr("Parse error line: ") + QString::number(l) + tr(", char: ") + QString::number(yy->error_char));
            emit generateMIDIFinished(1, cont);
        } else {
            AbcSmf *smf = new AbcSmf(yy, xspinbox.value(), this);

            if (!smf) {
                emit generateMIDIFinished(1, cont);
                abc_release_yy(yy);
                return;
            }

            QString midi(tempFile.fileName());
            midi.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value()) + ".mid");

            smf->writeToFile(midi);

            delete smf;
            emit generateMIDIFinished(0, cont);
        }

        abc_release_yy(yy);
        return;
    }

    /* or abc2midi executable */
    QString program = player.toString();
    QStringList argv = program.split(" ");
    program = argv.at(0);

    if (!QFileInfo::exists(program)) {
        QMessageBox::warning(a->mainWindow(), tr("Error"), tr("Cannot generate MIDI: Please check settings."));
        playpushbutton.flip();
        xspinbox.setEnabled(true);
        playpushbutton.setEnabled(true);
        return;
    }

    argv.removeAt(0);
    argv << tempFile.fileName();
    argv << QString::number(xspinbox.value());
    QFileInfo info(tempFile.fileName());
    QDir dir = info.absoluteDir();

    spawnMIDIGenerator(program, argv, dir, cont);
}

void EditVBoxLayout::exportPostscript(const QString &filename)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    a->mainWindow()->statusBar()->showMessage(tr("Exporting score..."));
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();

    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant param = settings.value(PSTUNES_KEY);

    QString program("abcm2ps");
    QStringList argv = program.split(" ");

    if (param.toString() == TUNES_ALL) {
        argv << "-N1" << "-O" << filename << tempFile.fileName();
    } else {
        argv << "-N1" << "-e" << QString::number(xspinbox.value()) << "-O" << filename << tempFile.fileName();
    }

    qWarning() << argv;

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
    emit compilerFinished(ret, 0);
#else
    argv.removeAt(0);
    spawnSVGCompiler(program, argv, dir, 0);
#endif
}

void EditVBoxLayout::onGenerateMIDIFinished(int exitCode, int cont)
{
    playpushbutton.setEnabled(true);
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    if (exitCode) {
        a->mainWindow()->statusBar()->showMessage(tr("Error during MIDI generation."));
        if (cont) {
            playpushbutton.flip();
            xspinbox.setEnabled(true);
        } else {
            QMessageBox::warning(a->mainWindow(), tr("Error"), tr("Error during MIDI generation."));
        }

        return;
    }

    a->mainWindow()->statusBar()->showMessage(tr("MIDI generation finished."));

    if (!cont) {
        /* move to export file name */
        QString midifile(tempFile.fileName());
        midifile.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid");

        QDir dir(tempFile.fileName());
        dir.remove(exportpath);
        if (!dir.rename(midifile, exportpath)) {
            QMessageBox::warning(a->mainWindow(), tr("Error"), tr("Cannot rename MIDI file."));
        }
    } else {
            playMIDI();
    }
}

void EditVBoxLayout::playMIDI() {
        if (synth->isPlaying())
                synth->waitFinish();

        /* midi file can change from tune (xspinbox) index */
        QString midifile(tempFile.fileName());
        midifile.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid");
        synth->play(midifile);
}

void EditVBoxLayout::onSynthFinished(bool err)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (err) {
        a->mainWindow()->statusBar()->showMessage(tr("Error during synthesis."));
    } else {
        a->mainWindow()->statusBar()->showMessage(tr("Synthesis finished."));
    }

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

    /* as we do not know the number of previous pages (it could be more or less)
     * we have to remove bruteforcely them */
    removeSvgFiles();

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
    emit compilerFinished(ret, 1);
#else
    spawnSVGCompiler(program, argv, dir, 1);
#endif
}

int EditVBoxLayout::xOfCursor(const QTextCursor& c) {
    int index = c.selectionStart();
    QString all = abcPlainTextEdit()->toPlainText();
    int x = 1;
    int i = 0;
    QStringList lines = all.split('\n');

    /* find last X: before selectionIndex */
    for (int l = 0; l < lines.count() && i < index; l++) {
        i += lines.at(l).count() +1; /* count \n */
        if (lines.at(l).startsWith("X:")) {
            x = lines.at(l).rightRef(1).toInt();
        }
    }

    return x;
}

void EditVBoxLayout::onSelectionChanged()
{
    QTextCursor c = abcPlainTextEdit()->textCursor();
    if (c.hasSelection()) {
        selection = c.selectedText();
        selectionIndex = c.selectionStart();
    } else {
        selection.clear();
        selectionIndex = c.selectionStart();
        /* set X spinbox */
        int x = xOfCursor(c);
        if (xspinbox.value() != x) {
            xspinbox.setValue(xOfCursor(c));
            /* refresh print */
            onRunClicked();
        }
    }
}

void EditVBoxLayout::onCompileFinished(int exitCode, int cont)
{
    qDebug() << "compile" << exitCode;

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (exitCode < 0 || exitCode > 1) { /* sometimes, abcm2ps returns 1 even on 'success' */
        if (cont) {
            a->mainWindow()->statusBar()->showMessage(tr("Error during score generation."));
        } else {
            QMessageBox::warning(a->mainWindow(), tr("Error"), tr("Error during score generation."));
        }

        return;
    }

    a->mainWindow()->statusBar()->showMessage(tr("Score generated."));

    if (!cont) {
        runpushbutton.setEnabled(true);
        return;
    }

    QFileInfo info(tempFile);
    QString b(info.baseName());
    QString d = info.dir().absolutePath();
    a->mainWindow()->mainHSplitter()->viewWidget()->cleanup();
    a->mainWindow()->mainHSplitter()->viewWidget()->initBasename(b, d);
    a->mainWindow()->mainHSplitter()->viewWidget()->requestPage(1);

    runpushbutton.setEnabled(true);
}
