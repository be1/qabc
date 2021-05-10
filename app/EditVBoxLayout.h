#ifndef EDITVBOXLAYOUT_H
#define EDITVBOXLAYOUT_H

#include "RunPushButton.h"
#include "PlayPushButton.h"
#include "AbcPlainTextEdit.h"
#include "AbcProcess.h"
#include "TuneWaiter.h"
#include "AbcTemporaryFile.h"
#include "sfloader.h"
#include "QProgressIndicator.h"
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
    void cleanupThreads();

    void spawnSVGCompiler(const QString &prog, const QStringList &args, const QDir& wrk, int cont);
    void spawnMIDIGenerator(const QString &prog, const QStringList& args, const QDir& wrk, int cont);
    void exportMIDI(const QString& filename);

    //static int handle_midi_event(void* data, fluid_midi_event_t *ev);

signals:
    void compilerFinished(int exitCode, int cont);
    void generateMIDIFinished(int exitCode, int cont);
    void doExportMIDI(const QString& outfilename);

protected:
    void spawnProgram(const QString& prog, const QStringList &args, AbcProcess::ProcessType which, const QDir &wrk, int cont);
    void removeSvgFiles();
    void removeMIDIFile();
    void playMIDI();
    int xOfCursor(const QTextCursor& c);

public slots:
    void onXChanged(int value);
    void onPlayClicked(); /* midi */
    void onRunClicked(); /* svg */
    void onSelectionChanged();

protected slots:
    void onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType, int cont);
    void onProgramOutputText(const QByteArray& text);
    void onProgramErrorText(const QByteArray& text);
    void onCompileFinished(int exitCode, int cont);
    void onGenerateMIDIFinished(int exitCode, int cont);
    void onSynthFinished(int exitCode);
    void onEarlySFLoadFinished(int fid);
    void onSFLoadFinished(int fid);

private:
	AbcPlainTextEdit abcplaintextedit;
    PlayPushButton playpushbutton; /* midi */
    RunPushButton runpushbutton; /* svg */
    QHBoxLayout hboxlayout;
    QSpinBox xspinbox;
    QLabel xlabel;
    QString fileName;
    QString exportpath;
    AbcTemporaryFile tempFile;
    QList<AbcProcess*> processlist;
    TuneWaiter *waiter;
    SFLoader *sfloader;
    QProgressIndicator* progress;
    QString selection;
    int selectionIndex;

    fluid_settings_t* fluid_settings = NULL;
    fluid_synth_t* fluid_synth = NULL;
    fluid_player_t* fluid_player = NULL;
    fluid_audio_driver_t* fluid_adriver = NULL;
    QString curSFont;
    int sfid = 0;
    char *id = NULL; /* jack identifier */
    char *drv = NULL; /* "alsa" or "pulseaudi"o or "jack" */
    char *sf = NULL; /* soundfont file name */
    char *mf = NULL; /* current midi file name */
};
#endif
