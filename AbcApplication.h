#ifndef ABCAPPLICATION_H
#define ABCAPPLICATION_H

#include "AbcMainWindow.h"
#include "AbcProcess.h"
#include <QApplication>
#include <QDir>

class AbcApplication: public QApplication
{
	Q_OBJECT

public:
	AbcApplication(int& argc, char **argv);
    ~AbcApplication();

    AbcMainWindow *mainWindow();
    void openFileNames(const QStringList& fileNames);
    void spawnCompiler(const QString &prog, const QStringList &args, const QDir& wrk);
    void spawnPlayer(const QString &prog, const QStringList& args, const QDir& wrk);
    void spawnSynth(const QString &prog, const QStringList &args, const QDir& wrk);

signals:
    void compilerFinished(int exitCode);
    void playerFinished(int exitCode);
    void synthFinished(int exitCode);

protected:
    void spawnProgram(const QString& prog, const QStringList &args, AbcProcess::ProcessType which, const QDir &wrk);

protected slots:
    void onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType);

private:
    AbcMainWindow abcmainwindow;
    QList<AbcProcess*> processlist;
};
#endif
