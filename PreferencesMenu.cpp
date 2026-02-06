// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PreferencesMenu.h"
#include "AbcApplication.h"
#include "editorprefdialog.h"
#include "settings.h"
#include <QDebug>
#include <QInputDialog>

PreferencesMenu::PreferencesMenu(QWidget* parent)
    : QMenu(parent)
{
    setTitle(tr("Preferences"));


    addAction(tr("MIDI Generator"), this, &PreferencesMenu::onPlayerActionTriggered);
    addAction(tr("MIDI Synthesizer"), this, &PreferencesMenu::onSynthActionTriggered);
    addAction(tr("Score generator"), this, &PreferencesMenu::onCompilerActionTriggered);
    addAction(tr("Score viewer"), this, &PreferencesMenu::onViewerActionTriggered);
    addAction(tr("Editor settings"), this, &PreferencesMenu::onEditorActionTriggered);
    addAction(tr("Reset to defaults"), this, &PreferencesMenu::onResetActionTriggered);
}

PreferencesMenu::~PreferencesMenu()
{
}

void PreferencesMenu::onCompilerActionTriggered()
{
    Settings settings;
    QVariant compiler = settings.value(COMPILER_KEY);

    QInputDialog dlg(this);

    dlg.setWindowTitle(tr("Compiler preference"));
    dlg.setLabelText(tr("Compiler:"));
    dlg.setInputMode(QInputDialog::TextInput);

    QLineEdit *edit = dlg.findChild<QLineEdit *>();
    if (edit) edit->setMinimumWidth(400);

    QString command;

    if (!compiler.isNull())
        dlg.setTextValue(compiler.toString());
    else
        dlg.setTextValue(ABCM2PS);


    if (dlg.exec() == QDialog::Accepted) {
	    command = dlg.textValue();

	    settings.setValue(COMPILER_KEY, command);
	    settings.sync();
    }
}

void PreferencesMenu::onPlayerActionTriggered()
{
    Settings settings;
    QVariant player = settings.value(PLAYER_KEY);

    QInputDialog dlg(this);

    dlg.setWindowTitle(tr("Player preference"));
    dlg.setLabelText(tr("Player:"));
    dlg.setInputMode(QInputDialog::TextInput);

    QLineEdit *edit = dlg.findChild<QLineEdit *>();
    if (edit) edit->setMinimumWidth(400);

    QString command;

    if (!player.isNull())
        dlg.setTextValue(player.toString());
    else
        dlg.setTextValue(ABC2MIDI);


    if (dlg.exec() == QDialog::Accepted) {
	    command = dlg.textValue();

	    settings.setValue(PLAYER_KEY, command);
	    settings.sync();
    }
}

void PreferencesMenu::onSynthActionTriggered()
{
    Settings settings;
    QVariant synth = settings.value(SYNTH_KEY);

    QInputDialog dlg(this);

    dlg.setWindowTitle(tr("Synth preference"));
    dlg.setLabelText(tr("Synth:"));
    dlg.setInputMode(QInputDialog::TextInput);

    QLineEdit *edit = dlg.findChild<QLineEdit *>();
    if (edit) edit->setMinimumWidth(400);

    QString command;

    if (!synth.isNull())
        dlg.setTextValue(synth.toString());
    else
        dlg.setTextValue(FLUIDSYNTH);


    if (dlg.exec() == QDialog::Accepted) {
	    command = dlg.textValue();

	    settings.setValue(SYNTH_KEY, command);
	    settings.sync();
    }
}

void PreferencesMenu::onViewerActionTriggered()
{
    Settings settings;
    QVariant viewer = settings.value(VIEWER_KEY);

    QInputDialog dlg(this);

    dlg.setWindowTitle(tr("PS viewer preference"));
    dlg.setLabelText(tr("PS viewer:"));
    dlg.setInputMode(QInputDialog::TextInput);

    QLineEdit *edit = dlg.findChild<QLineEdit *>();
    if (edit) edit->setMinimumWidth(400);

    QString command;

    if (!viewer.isNull())
        dlg.setTextValue(viewer.toString());
    else
        dlg.setTextValue(PSVIEWER);


    if (dlg.exec() == QDialog::Accepted) {
	    command = dlg.textValue();

	    settings.setValue(VIEWER_KEY, command);
	    settings.sync();
    }
}

void PreferencesMenu::onEditorActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditorPrefDialog* dialog = new EditorPrefDialog(a->mainWindow());

    if (QDialog::Accepted == dialog->exec()) {
        Settings settings;

        settings.setValue(EDITOR_FONT_BASE, dialog->getBaseFont().family());
        settings.setValue(EDITOR_FONT_RANGE, dialog->getFontRange());
        settings.setValue(EDITOR_HIGHLIGHT, dialog->getHighlight());
        settings.setValue(EDITOR_BAR_COLOR, dialog->getColor(EDITOR_BAR_COLOR));
        settings.setValue(EDITOR_COMMENT_COLOR, dialog->getColor(EDITOR_COMMENT_COLOR));
        settings.setValue(EDITOR_DECORATION_COLOR, dialog->getColor(EDITOR_DECORATION_COLOR));
        settings.setValue(EDITOR_EXTRAINSTR_COLOR, dialog->getColor(EDITOR_EXTRAINSTR_COLOR));
        settings.setValue(EDITOR_GCHORD_COLOR, dialog->getColor(EDITOR_GCHORD_COLOR));
        settings.setValue(EDITOR_HEADER_COLOR, dialog->getColor(EDITOR_HEADER_COLOR));
        settings.setValue(EDITOR_LYRIC_COLOR, dialog->getColor(EDITOR_LYRIC_COLOR));
        settings.sync();
    }

    delete dialog;
}

void PreferencesMenu::onResetActionTriggered()
{
    Settings settings;

    settings.reset();

    settings.sync();
}
