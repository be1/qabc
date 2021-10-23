// SPDX-License-Identifier: GPL-3.0-or-later

#include "AbcProcess.h"
#include <QDebug>

AbcProcess::AbcProcess(ProcessType which, QObject *parent)
    : QProcess(parent)
{
    type = which;
    setProcessChannelMode(SeparateChannels);
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &AbcProcess::onFinished);
    connect(this, &QProcess::errorOccurred, this, &AbcProcess::onErrorOccured);
#if 0
    connect(this, &QProcess::readyRead, this, &AbcProcess::onOutput);
#else
    connect(this, &QProcess::readyReadStandardOutput, this, &AbcProcess::onStdout);
    connect(this, &QProcess::readyReadStandardError, this, &AbcProcess::onStderr);
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
#if 0
void AbcProcess::onOutput()
{
    output = readAll();
    qDebug() << output;
    emit outputText(output);
}
#else
void AbcProcess::onStdout()
{
    emit outputText(readAllStandardOutput());
    /*
    while(canReadLine()) {
        emit outputText(readLine());
    }
    */
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

void AbcProcess::onErrorOccured(QProcess::ProcessError error)
{
    emit errorOccurred(error, program(), which());
}
