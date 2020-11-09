#include "AbcProcess.h"
#include <QDebug>

AbcProcess::AbcProcess(ProcessType which, QObject *parent)
    : QProcess(parent)
{
    type = which;
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &AbcProcess::onFinished);
    connect(this, &QProcess::readyRead, this, &AbcProcess::onOutput);
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
}

void AbcProcess::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit finished(exitCode, exitStatus, type);
}
