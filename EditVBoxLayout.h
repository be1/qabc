#ifndef EDITVBOXLAYOUT_H
#define EDITVBOXLAYOUT_H

#include "RunPushButton.h"
#include "PlayPushButton.h"
#include "AbcPlainTextEdit.h"
#include "AbcProcess.h"
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
    void onXChanged(int value);
    void onPlayClicked();
    void onRunClicked();

    void onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType);
    void onPlayFinished(int exitCode);
    void onSynthFinished(int exitCode);
    void onCompileFinished(int exitCode);

private:
	AbcPlainTextEdit abcplaintextedit;
    PlayPushButton playpushbutton;
    RunPushButton runpushbutton;
    QHBoxLayout hboxlayout;
    QSpinBox xspinbox;
    QLabel xlabel;
    QString fileName;
    QList<AbcProcess*> processlist;
};
#endif
