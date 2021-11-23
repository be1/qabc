// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ABCPROCESS_H
#define ABCPROCESS_H

#include <QProcess>

class AbcProcess : public QProcess
{
    Q_OBJECT
public:
    enum ProcessType {ProcessUnknown, ProcessCompiler, ProcessViewer, ProcessPlayer, ProcessSynth};
    explicit AbcProcess(ProcessType which, QObject *parent);
    ProcessType which();
    QByteArray* log();

signals:
    void finished(int exitCode, QProcess::ExitStatus exitStatus, ProcessType which);
    void errorOccurred(QProcess::ProcessError error, const QString& program, ProcessType which);
    void outputText(const QByteArray& text);
#if 1
    void errorText(const QByteArray& text);
#endif

protected slots:
#if 0
    void onOutput();
#else
    void onStdout();
    void onStderr();
#endif
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onErrorOccured(QProcess::ProcessError error);

private:
    ProcessType type;
    QByteArray output;
};

#endif // ABCPROCESS_H
