#include "AbcProcess.h"
#include <QDebug>

AbcProcess::AbcProcess(ProcessType which, QObject *parent)
    : QProcess(parent)
{
    type = which;
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &AbcProcess::onFinished);
    connect(this, &QProcess::readyRead, this, &AbcProcess::onOutput);
#if 0
    connect(this, &QProcess::readyReadStandardOutput, this, &AbcProcess::onStdout);
    connect(this, &QProcess::readAllStandardError, this, &AbcProcess::onStderr);
#endif
}

AbcProcess::ProcessType AbcProcess::which()
{
    return type;
}

QByteArray *AbcProcess::log()
{
   return &output;
}

void AbcProcess::onOutput()
{
    output = readAll();
    qDebug() << output;
    emit outputText(output);
}
#if 0
void AbcProcess::onStdout()
{
    emit outputText(readAllStandardOutput());
}

void AbcProcess::onStderr()
{
   emit errorText(readAllStandardError());
}
#endif
void AbcProcess::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit finished(exitCode, exitStatus, type);
}
