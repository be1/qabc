// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PREFERENCESMENU_H
#define PREFERENCESMENU_H

#include <QAction>
#include <QMenu>
#include <config.h>

class PreferencesMenu: public QMenu
{
	Q_OBJECT

public:
	PreferencesMenu(QWidget* parent = nullptr);
	~PreferencesMenu();

protected slots:
    void onCompilerActionTriggered();
    void onPlayerActionTriggered();
    void onSynthActionTriggered();
    void onResetActionTriggered();
    void onViewerActionTriggered();
    void onEditorActionTriggered();

private:
    QAction compileraction;
    QAction playeraction;
    QAction synthaction;
    QAction resetaction;
    QAction vieweraction;
    QAction editoraction;
};
#endif
