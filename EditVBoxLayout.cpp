#include "EditVBoxLayout.h"
#include "AbcApplication.h"
#include "PreferencesMenu.h"
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QSpinBox>
#include <QDir>

EditVBoxLayout::EditVBoxLayout(const QString& fileName, QWidget* parent)
	: QVBoxLayout(parent),
	abcplaintextedit(parent),
	playpushbutton(parent),
	runpushbutton(parent),
	hboxlayout(parent),
	xspinbox(parent),
	xlabel(parent),
	fileName(fileName)
{
	setObjectName("EditVBoxLayout:" + fileName);
    tempFile.setFileTemplate(".XXXXXX.abc");
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
	connect(this, &EditVBoxLayout::synthFinished, this, &EditVBoxLayout::onSynthFinished);
	connect(this, &EditVBoxLayout::compilerFinished, this, &EditVBoxLayout::onCompileFinished);
    connect(this, &EditVBoxLayout::viewerFinished, this, &EditVBoxLayout::onViewFinished);
}


EditVBoxLayout::~EditVBoxLayout()
{
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

void EditVBoxLayout::onXChanged(int value)
{
	qDebug() << value;
}

void EditVBoxLayout::spawnCompiler(const QString &prog, const QStringList& args, const QDir &wrk)
{
	AbcApplication* a = static_cast<AbcApplication*>(qApp);
	CentralWidget* cw =  static_cast<CentralWidget*>(a->mainWindow()->centralWidget());
	cw->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
	return spawnProgram(prog, args, AbcProcess::ProcessCompiler, wrk);
}

void EditVBoxLayout::spawnViewer(const QString &prog, const QStringList &args, const QDir &wrk)
{
    return spawnProgram(prog, args, AbcProcess::ProcessViewer, wrk);
}

void EditVBoxLayout::spawnPlayer(const QString& prog, const QStringList &args, const QDir &wrk)
{
	AbcApplication* a = static_cast<AbcApplication*>(qApp);
	CentralWidget* cw =  static_cast<CentralWidget*>(a->mainWindow()->centralWidget());
	cw->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
	return spawnProgram(prog, args, AbcProcess::ProcessPlayer, wrk);
}

void EditVBoxLayout::spawnSynth(const QString &prog, const QStringList& args, const QDir& wrk)
{
	return spawnProgram(prog, args, AbcProcess::ProcessSynth, wrk);
}

void EditVBoxLayout::spawnProgram(const QString& prog, const QStringList& args, AbcProcess::ProcessType which, const QDir& wrk)
{
	AbcProcess *process = new AbcProcess(which, this);
	process->setWorkingDirectory(wrk.absolutePath());
	connect(process, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType>::of(&AbcProcess::finished), this, &EditVBoxLayout::onProgramFinished);
	processlist.append(process);
	qDebug() << prog << args;
	process->start(prog, args);
}

void EditVBoxLayout::onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType which)
{
	//qDebug() << exitStatus;
    switch (which) {
    case AbcProcess::ProcessPlayer:
        emit playerFinished(exitCode); break;
    case AbcProcess::ProcessCompiler:
        emit compilerFinished(exitCode); break;
    case AbcProcess::ProcessSynth:
        emit synthFinished(exitCode); break;
    case AbcProcess::ProcessViewer:
        emit viewerFinished(exitCode); break;
    case AbcProcess::ProcessUnknown:
    default:
        break;
    }

	/* delete garbage */
	for (int i = 0; i < processlist.length(); i++) {
		AbcProcess* proc = processlist.at(i);
		//qDebug() << proc->state();
		if (proc->state() == QProcess::NotRunning
				&& proc->exitCode() == exitCode
				&& proc->exitStatus() == exitStatus
				&& proc->which() == which) {
			QString str = QString::fromUtf8(*proc->log());
			AbcApplication* a = static_cast<AbcApplication*>(qApp);
			CentralWidget* cw =  static_cast<CentralWidget*>(a->mainWindow()->centralWidget());
			cw->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->appendPlainText(str);
			disconnect(proc, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType>::of(&AbcProcess::finished), this, &EditVBoxLayout::onProgramFinished);
			delete proc;
			processlist.removeAt(i);
		}
	}
}

void EditVBoxLayout::onPlayClicked()
{
    playpushbutton.setEnabled(false);
    xspinbox.setEnabled(false);
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();
	QSettings settings("Herewe", "QAbc");
	QVariant player = settings.value(PLAYER_KEY);
	QString program = player.toString();
	QStringList argv = program.split(" ");
	program = argv.at(0);
	argv.removeAt(0);
    argv << tempFile.fileName();
    argv << QString::number(xspinbox.value());
    QFileInfo info(tempFile.fileName());
	QDir dir = info.absoluteDir();

	spawnPlayer(program, argv, dir);
}

void EditVBoxLayout::onPlayFinished(int exitCode)
{
    qDebug() << "play" << exitCode;
	if (exitCode) {
        playpushbutton.setEnabled(true);
        xspinbox.setEnabled(true);
		return;
	}

	QSettings settings("Herewe", "QAbc");
	QVariant synth = settings.value(SYNTH_KEY);
	QString program = synth.toString();
	QStringList argv = program.split(" ");
	program = argv.at(0);
	argv.removeAt(0);
    QString temp(tempFile.fileName());
    argv << (temp.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid"));

	QFileInfo info(temp);
	QDir dir = info.absoluteDir();

	spawnSynth(program, argv, dir);

}

void EditVBoxLayout::onSynthFinished(int exitCode)
{
    qDebug() << "synth" << exitCode;

    int x = xspinbox.value();
    QString mid (tempFile.fileName());
    mid.replace(QRegularExpression("\\.abc$"), QString::number(x) + ".mid");
    QFile::remove(mid);

    playpushbutton.setEnabled(true);
    xspinbox.setEnabled(true);
}

void EditVBoxLayout::onRunClicked()
{
	runpushbutton.setEnabled(false);
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();
    QSettings settings("Herewe", "QAbc");
	QVariant compiler = settings.value(COMPILER_KEY);
	QString program = compiler.toString();
	QStringList argv = program.split(" ");
	program = argv.at(0);
	argv.removeAt(0);
    argv << tempFile.fileName();

    QFileInfo info(tempFile.fileName());
	QDir dir = info.absoluteDir();

	spawnCompiler(program, argv, dir);
}

void EditVBoxLayout::onCompileFinished(int exitCode)
{
    qDebug() << "compile" << exitCode;
    if (exitCode) {
        runpushbutton.setEnabled(true);
        return;
    }

    QSettings settings("Herewe", "QAbc");
    QVariant synth = settings.value(VIEWER_KEY);
    QString program = synth.toString();
    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    QString temp(tempFile.fileName());
    argv << (temp.replace(QRegularExpression("\\.abc$"), ".ps"));

    QFileInfo info(temp);
    QDir dir = info.absoluteDir();

    spawnViewer(program, argv, dir);
}

void EditVBoxLayout::onViewFinished(int exitCode)
{
    qDebug() << "viewer" << exitCode;

    QString ps (tempFile.fileName());
    ps.replace(QRegularExpression("\\.abc$"), ".ps");
    QFile::remove(ps);

    runpushbutton.setEnabled(true);
}
