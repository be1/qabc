#ifndef EDITVBOXLAYOUT_H
#define EDITVBOXLAYOUT_H

#include "RunPushButton.h"
#include "PlayPushButton.h"
#include "AbcPlainTextEdit.h"
#include "AbcProcess.h"
#include "AbcTemporaryFile.h"
#include "abcsynth.h"
#include "psgenerator.h"
#include "svggenerator.h"
#include "midigenerator.h"
#include "QProgressIndicator.h"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QDir>
#include <QTemporaryFile>
#include <QThread>

class EditVBoxLayout: public QVBoxLayout
{
	Q_OBJECT

public:
    explicit EditVBoxLayout(const QString& fileName, QWidget* parent = nullptr);
	~EditVBoxLayout();

    void finalize(void);
    AbcPlainTextEdit *abcPlainTextEdit();
    PlayPushButton *playPushButton();
    RunPushButton *runPushButton();
    void setFileName(const QString& fn);

    void exportPostscript(const QString& filename);
    void exportMIDI(const QString& filename);

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
    void cleanupProcesses();
    void cleanupThreads();

public slots:
    void onXChanged(int value);
    void onPlayClicked(); /* midi */
    void onRunClicked(); /* svg */
    void onSelectionChanged();

protected slots:
    void onCompileFinished(int exitCode, int cont);
    void onGenerateMIDIFinished(int exitCode, int cont);
    void onSynthInited(bool err);
    void onSynthFinished(bool err);

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
    QProgressIndicator* progress;
    QString selection;
    int selectionIndex;

    AbcSynth* synth;
    PsGenerator psgen;
    SvgGenerator svggen;
    MidiGenerator midigen;
};
#endif
