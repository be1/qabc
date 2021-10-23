/*
 *   This file is part of QAbc.
 *
 *   QAbc is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   QAbc is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with QAbc.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef EDITVBOXLAYOUT_H
#define EDITVBOXLAYOUT_H

#include "RunPushButton.h"
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
    RunPushButton *runPushButton();
    void setFileName(const QString& fn);
    void cleanup();

    void spawnCompiler(const QString &prog, const QStringList &args, const QDir& wrk);
    void spawnViewer(const QString &prog, const QStringList &args, const QDir& wrk);

    void spawnPlayer(const QString &prog, const QStringList& args, const QDir& wrk);
    void spawnSynth(const QString &prog, const QStringList &args, const QDir& wrk);

signals:
    void compilerFinished(int exitCode);
    void viewerFinished(int exitCode);
    void playerFinished(int exitCode);
    void synthFinished(int exitCode);
    void doExportMIDI();

protected:
    void spawnProgram(const QString& prog, const QStringList &args, AbcProcess::ProcessType which, const QDir &wrk);
    void killSynth();
    bool checkViewer();
    int xOfCursor(const QTextCursor& c);

protected slots:
    void onXChanged(int value);
    void onPlayClicked(); /* midi */
    void onRunClicked(); /* ps */
    void onSelectionChanged();

    void exportMIDI();
    void onErrorOccurred(QProcess::ProcessError error, const QString& program, AbcProcess::ProcessType);
    void onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType);
    void onProgramOutputText(const QByteArray& text);
    void onProgramErrorText(const QByteArray& text);
    void onPlayFinished(int exitCode);
    void onSynthFinished(int exitCode);
    void onCompileFinished(int exitCode);
    void onViewFinished(int exitCode);

private:
    AbcPlainTextEdit abcplaintextedit;
    PlayPushButton playpushbutton; /* midi */
    RunPushButton runpushbutton; /* ps */
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
