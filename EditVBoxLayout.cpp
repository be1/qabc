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
#include "PreferencesMenu.h"
#include "settings.h"
#include <QFileInfo>
#include <QDebug>
#include <QSpinBox>
#include <QDir>

EditVBoxLayout::EditVBoxLayout(const QString& fileName, QWidget* parent)
	: QVBoxLayout(parent),
	abcplaintextedit(parent),
	playpushbutton(parent),
	runpushbutton(parent),
	hboxlayout(parent),
	xspinbox(parent),
	xlabel(parent),
	fileName(fileName)
{
    setObjectName("EditVBoxLayout:" + fileName);
    tempFile.setFileTemplate(QDir::tempPath() + QDir::separator() + "qabc-XXXXXX.abc");
    xspinbox.setMinimum(1);
	xlabel.setText(tr("X:"));
	xlabel.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	xlabel.setBuddy(&xspinbox);
	hboxlayout.addWidget(&xlabel);
	hboxlayout.addWidget(&xspinbox);
	hboxlayout.addWidget(&playpushbutton);
	hboxlayout.addWidget(&runpushbutton);

	addWidget(&abcplaintextedit);
	addLayout(&hboxlayout);

    connect(&xspinbox, SIGNAL(valueChanged(int)), this, SLOT(onXChanged(int)));
    connect(&abcplaintextedit, &QPlainTextEdit::selectionChanged, this, &EditVBoxLayout::onSelectionChanged);
    connect(&playpushbutton, SIGNAL(clicked()), this, SLOT(onPlayClicked()));
    connect(&runpushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onRunClicked);

    connect(this, &EditVBoxLayout::doExportMIDI, this, &EditVBoxLayout::exportMIDI);
    connect(this, &EditVBoxLayout::playerFinished, this, &EditVBoxLayout::onPlayFinished);
    connect(this, &EditVBoxLayout::synthFinished, this, &EditVBoxLayout::onSynthFinished);
    connect(this, &EditVBoxLayout::compilerFinished, this, &EditVBoxLayout::onCompileFinished);
    connect(this, &EditVBoxLayout::viewerFinished, this, &EditVBoxLayout::onViewFinished);
}


EditVBoxLayout::~EditVBoxLayout()
{
#if 1
    /* kill slots could not to be called, so cleanup manually */
    QString temp(tempFile.fileName());
    temp.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid");
    if (QFileInfo::exists(temp))
        QDir().remove(temp);

    temp = tempFile.fileName();
    temp.replace(QRegularExpression("\\.abc$"), ".ps");
    if (QFileInfo::exists(temp))
        QDir().remove(temp);
#endif
}

int EditVBoxLayout::xOfCursor(const QTextCursor& c) {
    int index = c.selectionStart();
    QString all = abcPlainTextEdit()->toPlainText();
    int x = 1;
    int i = 0;
    QStringList lines = all.split('\n');

    /* find last X: before selectionIndex */
    for (int l = 0; l < lines.count() && i < index; l++) {
        i += lines.at(l).count() +1; /* count \n */
        if (lines.at(l).startsWith("X:")) {
            x = lines.at(l).right(1).toInt();
        }
    }

    return x;
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
        /* set X spinbox */
        int x = xOfCursor(c);
        xspinbox.setValue(x);
    }
}

void EditVBoxLayout::exportMIDI() {
    QString tosave;

    if (selection.isEmpty()) {
        tosave = abcPlainTextEdit()->toPlainText();
    } else {
        QString all = abcPlainTextEdit()->toPlainText();
        int i = 0, xl = 0;
        QStringList lines = all.split('\n');

        /* find last X: before selectionIndex */
        for (int l = 0; l < lines.count() && i < selectionIndex; l++) {
            i += lines.at(l).count() +1; /* count \n */
            if (lines.at(l).startsWith("X:")) {
                xspinbox.setValue(lines.at(l).right(1).toInt());
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

    /* open tempfile to init a name */
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();

#if 0
    int cont;
    if (filename.isEmpty()) {
        cont = 1; /* continue to playback */
    } else {
        cont = 0; /* will not play, it's just an export */
    }
#endif

    Settings settings;
    QVariant player = settings.value(PLAYER_KEY);
    QString program = player.toString();
    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    argv << tempFile.fileName();
    argv << QString::number(xspinbox.value());
    QFileInfo info(tempFile.fileName());
    QDir dir = info.absoluteDir();

    spawnPlayer(program, argv, dir);
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
    case AbcProcess::ProcessPlayer:
    case AbcProcess::ProcessSynth:
        playpushbutton.flip();
        xspinbox.setEnabled(true);
        break;
    case AbcProcess::ProcessCompiler:
        runpushbutton.setEnabled(true);
        break;
    case AbcProcess::ProcessViewer:
        runpushbutton.setText(tr("&View score"));
        runpushbutton.setEnabled(true);
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

RunPushButton *EditVBoxLayout::runPushButton()
{
	return &runpushbutton;
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
	qDebug() << value;
}

void EditVBoxLayout::spawnCompiler(const QString &prog, const QStringList& args, const QDir &wrk)
{
	AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
	return spawnProgram(prog, args, AbcProcess::ProcessCompiler, wrk);
}

void EditVBoxLayout::spawnViewer(const QString &prog, const QStringList &args, const QDir &wrk)
{
    return spawnProgram(prog, args, AbcProcess::ProcessViewer, wrk);
}

void EditVBoxLayout::spawnPlayer(const QString& prog, const QStringList &args, const QDir &wrk)
{
	AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow *w = a->mainWindow();
    w->mainHBoxLayout()->viewWidget()->viewVBoxLayout()->logView()->clear();
	return spawnProgram(prog, args, AbcProcess::ProcessPlayer, wrk);
}

void EditVBoxLayout::spawnSynth(const QString &prog, const QStringList& args, const QDir& wrk)
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
    case AbcProcess::ProcessPlayer:
        emit playerFinished(exitCode); break;
    case AbcProcess::ProcessCompiler:
        emit compilerFinished(exitCode); break;
    case AbcProcess::ProcessSynth:
        emit synthFinished(exitCode); break;
    case AbcProcess::ProcessViewer:
        emit viewerFinished(exitCode); break;
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
			delete proc;
			processlist.removeAt(i);
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
            delete proc;
            processlist.removeAt(i);
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
    if (playpushbutton.isPlay()) {
        a->mainWindow()->statusBar()->showMessage(tr("Generating MIDI for playing."));
        playpushbutton.flip();
        xspinbox.setEnabled(false);
        emit doExportMIDI();
    } else {
        a->mainWindow()->statusBar()->showMessage(tr("Stopping synthesis."));
        killSynth();
		onSynthFinished(0);
    }
}

void EditVBoxLayout::onPlayFinished(int exitCode)
{
    qDebug() << "play" << exitCode;
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (exitCode) {
        a->mainWindow()->statusBar()->showMessage(tr("Error during MIDI generation."));
        playpushbutton.flip();
        xspinbox.setEnabled(true);
		return;
    }

    a->mainWindow()->statusBar()->showMessage(tr("MIDI generation finished."));

    Settings settings;
	QVariant synth = settings.value(SYNTH_KEY);
	QString program = synth.toString();
	QStringList argv = program.split(" ");
	program = argv.at(0);
	argv.removeAt(0);
    QString temp(tempFile.fileName());
    argv << (temp.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid"));

	QFileInfo info(temp);
	QDir dir = info.absoluteDir();

    a->mainWindow()->statusBar()->showMessage(tr("Starting synthesis..."));
    spawnSynth(program, argv, dir);

}

void EditVBoxLayout::onSynthFinished(int exitCode)
{
    qDebug() << "synth" << exitCode;

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    a->mainWindow()->statusBar()->showMessage(tr("Synthesis finished."));

    int x = xspinbox.value();
    QString mid (tempFile.fileName());
    mid.replace(QRegularExpression("\\.abc$"), QString::number(x) + ".mid");
    QFile::remove(mid);

    if (!playPushButton()->isPlay()) {
        playpushbutton.flip();
        xspinbox.setEnabled(true);
    }
}

void EditVBoxLayout::onRunClicked()
{
    runpushbutton.setEnabled(false);
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    a->mainWindow()->statusBar()->showMessage(tr("Generating score..."));
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();
    Settings settings;
	QVariant compiler = settings.value(COMPILER_KEY);
	QString program = compiler.toString();
	QStringList argv = program.split(" ");
	program = argv.at(0);
	argv.removeAt(0);
    argv << tempFile.fileName();

    QFileInfo info(tempFile.fileName());
	QDir dir = info.absoluteDir();

	spawnCompiler(program, argv, dir);
}

void EditVBoxLayout::onCompileFinished(int exitCode)
{
    qDebug() << "compile" << exitCode;

    runpushbutton.setText(tr("Refresh &view"));
    runpushbutton.setEnabled(true);

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (exitCode < 0 || exitCode > 1) { /* sometimes, abcm2ps returns 1 even on 'success' */
        a->mainWindow()->statusBar()->showMessage(tr("Error during score generation."));
        return;
    }

    a->mainWindow()->statusBar()->showMessage(tr("Score generated."));


    if (checkViewer())
        return;

    Settings settings;
    QVariant synth = settings.value(VIEWER_KEY);
    QString program = synth.toString();
    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    QString temp(tempFile.fileName());
    argv << (temp.replace(QRegularExpression("\\.abc$"), ".ps"));

    QFileInfo info(temp);
    QDir dir = info.absoluteDir();

    a->mainWindow()->statusBar()->showMessage(tr("Starting viewer..."));
    spawnViewer(program, argv, dir);
}

void EditVBoxLayout::onViewFinished(int exitCode)
{
    qDebug() << "viewer" << exitCode;

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    a->mainWindow()->statusBar()->showMessage(tr("Viewer closed."));

    Settings settings;
    QString ps (tempFile.fileName());
    ps.replace(QRegularExpression("\\.abc$"), ".ps");
    QFile::remove(ps);

    runpushbutton.setText(tr("&View score"));
}
