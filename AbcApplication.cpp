#include "AbcApplication.h"
#include <QSettings>
#include <QDebug>
#include "config.h"

AbcApplication::AbcApplication(int& argc, char **argv)
	: QApplication(argc, argv)
{
	setOrganizationName("Herewe");
	setOrganizationDomain("herewe");
    setApplicationName("QAbc");
    setApplicationVersion(VERSION " (" REVISION ")");

    QSettings settings("Herewe", "QAbc");

    QVariant compiler = settings.value(COMPILER_KEY);
    if (!compiler.isValid())
        settings.setValue(COMPILER_KEY, ABCM2PS);

    QVariant player = settings.value(PLAYER_KEY);
    if (!player.isValid())
        settings.setValue(PLAYER_KEY, ABC2MMIDI);

    QVariant synth = settings.value(SYNTH_KEY);
    if (!synth.isValid())
        settings.setValue(SYNTH_KEY, FLUIDSYNTH);

    settings.sync();
}

AbcApplication::~AbcApplication()
{
}

AbcMainWindow *AbcApplication::mainWindow()
{
    return &abcmainwindow;
}

void AbcApplication::openFileNames(const QStringList &fileNames)
{
    CentralWidget *cw = static_cast<CentralWidget*>(mainWindow()->centralWidget());
    EditTabWidget* edittabs = cw->mainHBoxLayout()->editTabWidget();
    for (int i = 0; i < fileNames.length(); i++) {
        QString fileName = fileNames[i];
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            EditWidget* widget = new EditWidget(fileName, edittabs);
            edittabs->addTab(widget);
            AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
            edit->setPlainText(file.readAll());
            file.close();
        }
    }
}

void AbcApplication::spawnCompiler(const QString &prog, const QStringList& args, const QDir &wrk)
{
    CentralWidget* cw =  static_cast<CentralWidget*>(abcmainwindow.centralWidget());
    cw->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessCompiler, wrk);
}

void AbcApplication::spawnPlayer(const QString& prog, const QStringList &args, const QDir &wrk)
{
    CentralWidget* cw =  static_cast<CentralWidget*>(abcmainwindow.centralWidget());
    cw->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessPlayer, wrk);
}

void AbcApplication::spawnSynth(const QString &prog, const QStringList& args, const QDir& wrk)
{
    return spawnProgram(prog, args, AbcProcess::ProcessSynth, wrk);
}

void AbcApplication::spawnProgram(const QString& prog, const QStringList& args, AbcProcess::ProcessType which, const QDir& wrk)
{
    AbcProcess *process = new AbcProcess(which, this);
    process->setWorkingDirectory(wrk.absolutePath());
    connect(process, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType>::of(&AbcProcess::finished), this, &AbcApplication::onProgramFinished);
    processlist.append(process);
    qDebug() << prog << args;
    process->start(prog, args);
}

void AbcApplication::onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType which)
{
    //qDebug() << exitStatus;
    switch (which) {
    case AbcProcess::ProcessPlayer:
        emit playerFinished(exitCode); break;
    case AbcProcess::ProcessCompiler:
        emit compilerFinished(exitCode); break;
    case AbcProcess::ProcessSynth:
        emit synthFinished(exitCode); break;
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
            CentralWidget* cw =  static_cast<CentralWidget*>(abcmainwindow.centralWidget());
            cw->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->appendPlainText(str);
            disconnect(proc, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType>::of(&AbcProcess::finished), this, &AbcApplication::onProgramFinished);
            proc->kill();
            delete proc;
            processlist.removeAt(i);
        }
    }
}
