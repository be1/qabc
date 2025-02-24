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
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    Settings settings;
    QVariant compiler = settings.value(COMPILER_KEY);

    bool ok;
    QString command;
    if (!compiler.isNull())
        command = QInputDialog::getText(a->mainWindow(), tr("Compiler preference"), tr("Compiler:"), QLineEdit::Normal, compiler.toString(), &ok);
    else
        command = QInputDialog::getText(a->mainWindow(), tr("Compiler preference"), tr("Compiler:"), QLineEdit::Normal, ABCM2PS, &ok);

    if (!ok)
        return;

    settings.setValue(COMPILER_KEY, command);
    settings.sync();
}

void PreferencesMenu::onPlayerActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    Settings settings;
    QVariant player = settings.value(PLAYER_KEY);

    bool ok;
    QString command;
    if (!player.isNull())
        command = QInputDialog::getText(a->mainWindow(), tr("Player preference"), tr("Player:"), QLineEdit::Normal, player.toString(), &ok);
    else
        command = QInputDialog::getText(a->mainWindow(), tr("Player preference"), tr("Player:"), QLineEdit::Normal, ABC2MIDI, &ok);

    if (!ok)
        return;

    settings.setValue(PLAYER_KEY, command);
    settings.sync();
}

void PreferencesMenu::onSynthActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    Settings settings;
    QVariant synth = settings.value(SYNTH_KEY);

    bool ok;
    QString command;
    if (!synth.isNull())
        command = QInputDialog::getText(a->mainWindow(), tr("Synth preference"), tr("Synth:"), QLineEdit::Normal, synth.toString(), &ok);
    else
        command = QInputDialog::getText(a->mainWindow(), tr("Synth preference"), tr("Synth:"), QLineEdit::Normal, FLUIDSYNTH, &ok);

    if (!ok)
        return;

    settings.setValue(SYNTH_KEY, command);
    settings.sync();
}

void PreferencesMenu::onViewerActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    Settings settings;
    QVariant viewer = settings.value(VIEWER_KEY);

    bool ok;
    QString command;
    if (!viewer.isNull())
        command = QInputDialog::getText(a->mainWindow(), tr("PS viewer preference"), tr("PS Viewer:"), QLineEdit::Normal, viewer.toString(), &ok);
    else
        command = QInputDialog::getText(a->mainWindow(), tr("PS viewer preference"), tr("PS Viewer:"), QLineEdit::Normal, PSVIEWER, &ok);

    if (!ok)
        return;

    settings.setValue(VIEWER_KEY, command);
    settings.sync();
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
