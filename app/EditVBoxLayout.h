#ifndef EDITVBOXLAYOUT_H
#define EDITVBOXLAYOUT_H

#include "RunPushButton.h"
#include "PlayPushButton.h"
#include "AbcPlainTextEdit.h"
#include "AbcProcess.h"
#include "TuneWaiter.h"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QDir>
#include <QTemporaryFile>
#include <QThread>
#include <fluidsynth.h>

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
    void cleanupProcesses();

    void spawnCompiler(const QString &prog, const QStringList &args, const QDir& wrk);
    void spawnPlayer(const QString &prog, const QStringList& args, const QDir& wrk);

    //static int handle_midi_event(void* data, fluid_midi_event_t *ev);

signals:
    void compilerFinished(int exitCode);
    void playerFinished(int exitCode);

protected:
    void spawnProgram(const QString& prog, const QStringList &args, AbcProcess::ProcessType which, const QDir &wrk);

public slots:
    void onXChanged(int value);
    void onPlayClicked(); /* midi */
    void onRunClicked(); /* svg */

protected slots:
    void onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType);
    void onProgramOutputText(const QByteArray& text);
    void onProgramErrorText(const QByteArray& text);
    void onCompileFinished(int exitCode);
    void onPlayFinished(int exitCode);
    void onSynthFinished(int exitCode);

private:
	AbcPlainTextEdit abcplaintextedit;
    PlayPushButton playpushbutton; /* midi */
    RunPushButton runpushbutton; /* svg */
    QHBoxLayout hboxlayout;
    QSpinBox xspinbox;
    QLabel xlabel;
    QString fileName;
    QTemporaryFile tempFile;
    QList<AbcProcess*> processlist;
    TuneWaiter *waiter;

    fluid_settings_t* fluid_settings = NULL;
    fluid_synth_t* fluid_synth = NULL;
    fluid_player_t* fluid_player = NULL;
    fluid_audio_driver_t* fluid_adriver = NULL;
    int sfid = 0;
    char *id = NULL; /* jack identifier */
    char *drv = NULL; /* "alsa" or "pulseaudi"o or "jack" */
    char *sf = NULL; /* soundfont file name */
    char *mf = NULL; /* current midi file name */
};
#endif
