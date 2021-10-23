// SPDX-License-Identifier: GPL-3.0-or-later

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
