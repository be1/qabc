// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EDITVBOXLAYOUT_H
#define EDITVBOXLAYOUT_H

#include "ViewPushButton.h"
#include "PlayPushButton.h"
#include "AbcPlainTextEdit.h"
#include "AbcProcess.h"
#include "AbcTemporaryFile.h"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QDir>


class EditVBoxLayout: public QVBoxLayout
{
	Q_OBJECT

public:
    explicit EditVBoxLayout(const QString& fileName, QWidget* parent = nullptr);
	~EditVBoxLayout();

    AbcPlainTextEdit *abcPlainTextEdit();
    PlayPushButton *playPushButton();
    ViewPushButton *runPushButton();
    void setFileName(const QString& fn);
    void cleanup();

    void spawnCompilerPs(const QString &prog, const QStringList &args, const QDir& wrk);
    void spawnViewerPs(const QString &prog, const QStringList &args, const QDir& wrk);

    void spawnExportMIDI(const QString &prog, const QStringList& args, const QDir& wrk);
    void spawnExportPs(const QString &prog, const QStringList& args, const QDir& wrk);

    void spawnCompilerMIDI(const QString &prog, const QStringList& args, const QDir& wrk);
    void spawnSynthMIDI(const QString &prog, const QStringList &args, const QDir& wrk);

signals:
    void compilerPsFinished(int exitCode);
    void viewerPsFinished(int exitCode);
    void compilerMIDIFinished(int exitCode);
    void synthMIDIFinished(int exitCode);
    void doExportMIDI(const QString& = "");
    void doExportPs(const QString& = "");

protected:
    void spawnProgram(const QString& prog, const QStringList &args, AbcProcess::ProcessType which, const QDir &wrk);
    void killSynth();
    bool checkViewer();
    QString getAbcText();

protected slots:
    void onCursorPositionChanged();
    void onXChanged(int value);
    void onPlayClicked(); /* midi */
    void onCompileClicked(); /* ps */
    void onSelectionChanged();

    void exportMIDI(const QString& outFilename = "");
    void exportPs(const QString& outFilename = "");
    void onErrorOccurred(QProcess::ProcessError error, const QString& program, AbcProcess::ProcessType);
    void onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType);
    void onProgramOutputText(const QByteArray& text);
    void onProgramErrorText(const QByteArray& text);
    void onCompilerMIDIFinished(int exitCode);
    void onSynthMIDIFinished(int exitCode);
    void onCompilerPsFinished(int exitCode);
    void onViewerPsFinished(int exitCode);

private:
    bool in_cursor_position_changed = false;
    AbcPlainTextEdit abcplaintextedit;
    PlayPushButton playpushbutton; /* midi */
    ViewPushButton compilepushbutton; /* ps */
    QHBoxLayout hboxlayout;
    QSpinBox xspinbox;
    QLabel xlabel;
    QString fileName;
    AbcTemporaryFile tempFile;
    QList<AbcProcess*> processlist;
    QString selection;
    int selectionIndex;
};
#endif
