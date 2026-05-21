// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "EditVBoxLayout.h"
#include "AbcApplication.h"
#include "Settings.h"
#include <QFileInfo>
#include <QDebug>
#include <QSpinBox>
#include <QDir>

EditVBoxLayout::EditVBoxLayout(const QString& fileName, QWidget* parent)
    : QVBoxLayout(parent),
    abcplaintextedit(parent),
    playpushbutton(parent),
    compilepushbutton(parent),
    hboxlayout(parent),
    xspinbox(parent),
    xlabel(parent),
    fileName(fileName)
{
    Settings settings;
    setObjectName("EditVBoxLayout:" + fileName);
    tempFile.setFileTemplate(QDir::tempPath() + QDir::separator() + "qabc-XXXXXX.abc");
    xspinbox.setMinimum(1);
    xlabel.setText(tr("X:"));
    xlabel.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    xlabel.setBuddy(&xspinbox);
    hboxlayout.addWidget(&xlabel);
    hboxlayout.addWidget(&xspinbox);
    hboxlayout.addWidget(&playpushbutton);
    loopcheckbox = new QCheckBox(tr("loop"), parent);
    if (settings.value(LOOP_KEY).toBool())
        loopcheckbox->setCheckState(Qt::Checked);
    hboxlayout.addWidget(loopcheckbox);
    hboxlayout.addWidget(&compilepushbutton);

    addWidget(&abcplaintextedit);
    addLayout(&hboxlayout);

    connect(&xspinbox, &QSpinBox::valueChanged, this, &EditVBoxLayout::onXChanged);
    connect(loopcheckbox, &QCheckBox::checkStateChanged, this, &EditVBoxLayout::onLoopStateChanged);
    connect(&abcplaintextedit, &QPlainTextEdit::selectionChanged, this, &EditVBoxLayout::onSelectionChanged);
    connect(&abcplaintextedit, &QPlainTextEdit::cursorPositionChanged, this, &EditVBoxLayout::onCursorPositionChanged);
    connect(&playpushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onPlayClicked);
    connect(&compilepushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onCompileClicked);

    connect(this, &EditVBoxLayout::doExportMIDI, this, &EditVBoxLayout::exportMIDI);
    connect(this, &EditVBoxLayout::doExportPs, this, &EditVBoxLayout::exportPs);
    connect(this, &EditVBoxLayout::compilerMIDIFinished, this, &EditVBoxLayout::onCompilerMIDIFinished);
    connect(this, &EditVBoxLayout::synthMIDIFinished, this, &EditVBoxLayout::onSynthMIDIFinished);
    connect(this, &EditVBoxLayout::compilerPsFinished, this, &EditVBoxLayout::onCompilerPsFinished);
    connect(this, &EditVBoxLayout::viewerPsFinished, this, &EditVBoxLayout::onViewerPsFinished);
}


EditVBoxLayout::~EditVBoxLayout()
{
#if 1
    /* kill slots could not to be called, so cleanup manually */
    QString temp(tempFile.fileName());
    temp.replace(QRegularExpression("\\.abc$"), QString::number(currentmidi)  + ".mid");
    if (QFileInfo::exists(temp))
        QDir().remove(temp);

    temp = tempFile.fileName();
    temp.replace(QRegularExpression("\\.abc$"), ".ps");
    if (QFileInfo::exists(temp))
        QDir().remove(temp);
#endif
}

void EditVBoxLayout::onCursorPositionChanged()
{
    in_cursor_position_changed = true;
    AbcPlainTextEdit* te = qobject_cast<AbcPlainTextEdit*>(sender());
    QTextCursor tc = te->textCursor();
    int x = abcplaintextedit.currentXV('X');

    if (xspinbox.value() == x)
        in_cursor_position_changed = false;
    else
        xspinbox.setValue(x);
}

void EditVBoxLayout::onSelectionChanged()
{
    QTextCursor c = abcPlainTextEdit()->textCursor();
    if (c.hasSelection()) {
        selection = c.selectedText();
        selectionIndex = c.selectionStart();
    } else {
        selection.clear();
        selectionIndex = c.selectionStart();
    }
}

void EditVBoxLayout::onLoopStateChanged(Qt::CheckState state)
{
    Settings settings;
    if (state == Qt::Checked)
        settings.setValue(LOOP_KEY, true);
    else
        settings.setValue(LOOP_KEY, false);
}

QString EditVBoxLayout::getAbcText () {
    QString tosave;

    if (selection.isEmpty()) {
        tosave = abcplaintextedit.toPlainText();
    } else {
        QString all = abcplaintextedit.toPlainText();
        int i = 0, xl = 0;
        QStringList lines = all.split('\n');

        /* find last X: before selectionIndex */
        for (int l = 0; l < lines.count() && i < selectionIndex; l++) {
            i += lines.at(l).size() +1; /* count \n */
            if (lines.at(l).startsWith("X:")) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
                xspinbox.setValue(lines.at(l).midRef(2).toInt());
#else
                xspinbox.setValue(lines.at(l).mid(2).toInt());
#endif
                xl = l;
                /* don't break on first X: continue until selectionIndex */
            }
        }

        /* construct headers */
        for (int j = xl;  j < lines.count(); j++) {
            if (lines.at(j).contains(QRegularExpression("^((%[^\n]*)|([A-Z]:[^\n]+))$"))) {
                if (lines.at(j).startsWith("%%")) /* ignore MIDI instruction, use basic Piano */
                    continue;

                tosave += lines.at(j) + "\n";
            } else
                break;
        }

        /* when coming from QTextCursor::selectedText(), LF is replaced by U+2029! */
        tosave += selection.replace(QChar::ParagraphSeparator, "\n");
    }

    return tosave;
}

void EditVBoxLayout::exportPs(const QString &outFilename) {
    QString tosave = getAbcText();

    /* open tempfile to init a name */
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();

    Settings settings;
    QVariant compiler = settings.value(COMPILER_KEY);
    QString program = compiler.toString();
    if (program.isEmpty())
        program = ABCM2PS;

    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    argv << tempFile.fileName();
    argv << "-e" << QString::number(xspinbox.value());

    /* remove potential -O= */
    int idx = argv.indexOf("-O=");
    if (idx >= 0)
        argv.removeAt(idx);

    /* for export or visualize */
    if (!outFilename.isEmpty()) {
        argv << "-O" << outFilename;
    } else {
        argv << "-O=";
    }

    QFileInfo info(tempFile.fileName());
    QDir dir = info.absoluteDir();

    /* export of visualize */
    if (!outFilename.isEmpty())
        spawnExportPs(program, argv, dir);
    else
        spawnCompilerPs(program, argv, dir);
}

void EditVBoxLayout::exportMIDI(const QString &outFilename) {
    QString tosave = getAbcText();

    /* open tempfile to init a name */
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();

    Settings settings;
    QVariant player = settings.value(PLAYER_KEY);
    QString program = player.toString();
    if (program.isEmpty())
        program = ABC2MIDI;

    currentmidi = xspinbox.value();

    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    argv << tempFile.fileName();
    argv << QString::number(currentmidi);

    if (!outFilename.isEmpty())
        argv << "-o" << outFilename;

    QFileInfo info(tempFile.fileName());
    QDir dir = info.absoluteDir();

    if (!outFilename.isEmpty()) {
        spawnExportMIDI(program, argv, dir);
    } else {
        spawnCompilerMIDI(program, argv, dir);
    }
}

void EditVBoxLayout::onErrorOccurred(QProcess::ProcessError error, const QString& program, AbcProcess::ProcessType which)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    LogView* lv = w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView();
    switch (error) {
        case QProcess::FailedToStart:
            lv->appendHtml("<b style=\"color: red\">" + tr("Failed to start program: ") + program +
                    "<br />" + tr("Please check settings.") + "</b>");
            break;
        default:
            break;
    }

    switch (which) {
        case AbcProcess::ProcessCompilerMIDI:
        case AbcProcess::ProcessSynth:
            if (error == QProcess::FailedToStart) {
                /* reset state only on start fail, not on kill */
                playpushbutton.flip();
                xspinbox.setEnabled(true);
            }
            break;
        case AbcProcess::ProcessCompilerPs:
            compilepushbutton.setEnabled(true);
            break;
        case AbcProcess::ProcessViewer:
            compilepushbutton.setText(tr("&View score"));
            compilepushbutton.setEnabled(true);
            break;
        default:
            break;
    }
}

AbcPlainTextEdit *EditVBoxLayout::abcPlainTextEdit()
{
    return &abcplaintextedit;
}

PlayPushButton *EditVBoxLayout::playPushButton()
{
    return &playpushbutton;
}

ViewPushButton *EditVBoxLayout::viewPushButton()
{
    return &compilepushbutton;
}

void EditVBoxLayout::setFileName(const QString &fn)
{
    fileName = fn;
}

void EditVBoxLayout::cleanup()
{
    /* early kill in case  we quit the app brutally */
    for (int i = 0; i < processlist.length(); i++)
        processlist.at(i)->kill();
}

void EditVBoxLayout::onXChanged(int value)
{
    if (!in_cursor_position_changed) {
        abcPlainTextEdit()->findX(value);
    }

    in_cursor_position_changed = false;
}

void EditVBoxLayout::spawnCompilerPs(const QString &prog, const QStringList& args, const QDir &wrk)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessCompilerPs, wrk);
}

void EditVBoxLayout::spawnViewerPs(const QString &prog, const QStringList &args, const QDir &wrk)
{
    return spawnProgram(prog, args, AbcProcess::ProcessViewer, wrk);
}

void EditVBoxLayout::spawnExportPs(const QString &prog, const QStringList& args, const QDir& wrk) {
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow *w = a->mainWindow();
    w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessUnknown, wrk);
}

void EditVBoxLayout::spawnExportMIDI(const QString& prog, const QStringList &args, const QDir &wrk)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow *w = a->mainWindow();
    w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessUnknown, wrk);
}

void EditVBoxLayout::spawnCompilerMIDI(const QString& prog, const QStringList &args, const QDir &wrk)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow *w = a->mainWindow();
    w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessCompilerMIDI, wrk);
}

void EditVBoxLayout::spawnSynthMIDI(const QString &prog, const QStringList& args, const QDir& wrk)
{
    return spawnProgram(prog, args, AbcProcess::ProcessSynth, wrk);
}

void EditVBoxLayout::spawnProgram(const QString& prog, const QStringList& args, AbcProcess::ProcessType which, const QDir& wrk)
{
    AbcProcess *process = new AbcProcess(which, this);
    process->setWorkingDirectory(wrk.absolutePath());
    connect(process, QOverload<QProcess::ProcessError, const QString&, AbcProcess::ProcessType>::of(&AbcProcess::errorOccurred), this, &EditVBoxLayout::onErrorOccurred);
    connect(process, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType>::of(&AbcProcess::finished), this, &EditVBoxLayout::onProgramFinished);
    connect(process, &AbcProcess::outputText, this, &EditVBoxLayout::onProgramOutputText);
#if 1
    connect(process, &AbcProcess::errorText, this, &EditVBoxLayout::onProgramErrorText);
#endif
    processlist.append(process);
    qDebug() << prog << args;
    process->start(prog, args);
}

void EditVBoxLayout::onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType which)
{
    qDebug() << exitCode << exitStatus;
    switch (which) {
        case AbcProcess::ProcessCompilerMIDI:
            emit compilerMIDIFinished(exitCode); break;
        case AbcProcess::ProcessCompilerPs:
            emit compilerPsFinished(exitCode); break;
        case AbcProcess::ProcessSynth:
            emit synthMIDIFinished(exitCode); break;
        case AbcProcess::ProcessViewer:
            emit viewerPsFinished(exitCode); break;
        case AbcProcess::ProcessUnknown:
        default:
            break;
    }

    /* delete garbage */
    for (int i = 0; i < processlist.length(); i++) {
        AbcProcess* proc = processlist.at(i);
        //qDebug() << proc->state();
        if (proc->state() == QProcess::NotRunning
                && proc->exitCode() == exitCode
                && proc->exitStatus() == exitStatus
                && proc->which() == which) {
            disconnect(proc, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType>::of(&AbcProcess::finished), this, &EditVBoxLayout::onProgramFinished);
            disconnect(proc, QOverload<QProcess::ProcessError, const QString&, AbcProcess::ProcessType>::of(&AbcProcess::errorOccurred), this, &EditVBoxLayout::onErrorOccurred);
            delete proc;
            processlist.removeAt(i);
            break;
        }
    }
}

void EditVBoxLayout::onProgramOutputText(const QByteArray &text)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    LogView* lv = w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView();
    lv->appendHtml("<em>" + QString::fromUtf8(text).replace("\n", "<br />") + "</em>");
}

void EditVBoxLayout::onProgramErrorText(const QByteArray &text)
{
    //onProgramOutputText(text);
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    LogView* lv = w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView();
    lv->appendHtml("<b style=\"color: gray\">" + QString::fromUtf8(text).replace("\n", "<br />") + "</b>");
}

void EditVBoxLayout::killSynth()
{
    for (int i = 0; i < processlist.length(); i++) {
        AbcProcess* proc = processlist.at(i);
        if (proc->state() == QProcess::Running
                && proc->which() == AbcProcess::ProcessSynth) {
#if 0
            QString str = QString::fromUtf8(*proc->log());
            AbcApplication* a = static_cast<AbcApplication*>(qApp);
            AbcMainWindow* w =  a->mainWindow();
            LogView* lv = w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView();
            lv->appendPlainText(str);
#endif
            disconnect(proc, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType>::of(&AbcProcess::finished), this, &EditVBoxLayout::onProgramFinished);
            disconnect(proc, QOverload<QProcess::ProcessError, const QString&, AbcProcess::ProcessType>::of(&AbcProcess::errorOccurred), this, &EditVBoxLayout::onErrorOccurred);
            delete proc;
            processlist.removeAt(i);
            break;
        }
    }
}

bool EditVBoxLayout::checkViewer()
{
    for (int i = 0; i < processlist.length(); i++) {
        AbcProcess* proc = processlist.at(i);
        if (proc->which() == AbcProcess::ProcessViewer)
            return true;
    }

    return false;
}

void EditVBoxLayout::onPlayClicked()
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (!playpushbutton.isPlay()) {
        qDebug() << "PLAY PRESSED";
        /* has filpped to STOP => start playing */
        a->mainWindow()->statusBar()->showMessage(tr("Generating MIDI for playing."));
        xspinbox.setEnabled(false);
        emit doExportMIDI();
    } else {
        qDebug() << "STOP PRESSED";
        /* has flipped to PLAY => stop playing */
        a->mainWindow()->statusBar()->showMessage(tr("Stopping synthesis."));
        killSynth();
        emit synthMIDIFinished(0);
    }
}

void EditVBoxLayout::onCompilerMIDIFinished(int exitCode)
{
    qDebug() << "play" << exitCode;
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (exitCode) {
        a->mainWindow()->statusBar()->showMessage(tr("Error during MIDI generation."));
        return;
    }

    a->mainWindow()->statusBar()->showMessage(tr("MIDI generation finished."));

    Settings settings;
    QVariant synth = settings.value(SYNTH_KEY);
    QString program = synth.toString();
    if (program.isEmpty()) program = FLUIDSYNTH;

    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    QString temp(tempFile.fileName());
    argv << (temp.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid"));

    QFileInfo info(temp);
    QDir dir = info.absoluteDir();

    a->mainWindow()->statusBar()->showMessage(tr("Starting synthesis..."));
    spawnSynthMIDI(program, argv, dir);
}

void EditVBoxLayout::onSynthMIDIFinished(int exitCode)
{
    qDebug() << "synth" << exitCode;

    /* loop if no button pressed for stopping */
    if (!playPushButton()->isPlay() && loopcheckbox->isChecked()) {
        /* icon is STOP, no manual press */
        onCompilerMIDIFinished(0);
        return;
    }

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    a->mainWindow()->statusBar()->showMessage(tr("Synthesis finished."));

    QString mid (tempFile.fileName());
    mid.replace(QRegularExpression("\\.abc$"), QString::number(currentmidi) + ".mid");
    QFile::remove(mid);

    /* natural end */
    if (!playpushbutton.isPlay())
        playpushbutton.flip();

    xspinbox.setEnabled(true);
}

void EditVBoxLayout::onCompileClicked()
{
    compilepushbutton.setEnabled(false);
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    a->mainWindow()->statusBar()->showMessage(tr("Generating score..."));
    emit doExportPs();
}

void EditVBoxLayout::onCompilerPsFinished(int exitCode)
{
    qDebug() << "compile" << exitCode;

    compilepushbutton.setText(tr("Refresh &view"));
    compilepushbutton.setEnabled(true);

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (exitCode < 0 || exitCode > 1) { /* sometimes, abcm2ps returns 1 even on 'success' */
        a->mainWindow()->statusBar()->showMessage(tr("Error during score generation."));
        return;
    }

    a->mainWindow()->statusBar()->showMessage(tr("Score generated."));


    if (checkViewer())
        return;

    Settings settings;
    QVariant viewer = settings.value(VIEWER_KEY);
    QString program = viewer.toString();
    if (program.isEmpty())
        program = PSVIEWER;

    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    QString temp(tempFile.fileName());
    argv << (temp.replace(QRegularExpression("\\.abc$"), ".ps"));

    QFileInfo info(temp);
    QDir dir = info.absoluteDir();

    a->mainWindow()->statusBar()->showMessage(tr("Starting viewer..."));
    spawnViewerPs(program, argv, dir);
}

void EditVBoxLayout::onViewerPsFinished(int exitCode)
{
    qDebug() << "viewer" << exitCode;

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    a->mainWindow()->statusBar()->showMessage(tr("Viewer closed."));

    Settings settings;
    QString ps (tempFile.fileName());
    ps.replace(QRegularExpression("\\.abc$"), ".ps");
    QFile::remove(ps);

    compilepushbutton.setText(tr("&View score"));
}

// vim:ts=4:sw=4:et
