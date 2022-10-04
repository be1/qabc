// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ABCMAINWINDOW_H
#define ABCMAINWINDOW_H

#include "HelpMenu.h"
#include "PreferencesMenu.h"
#include "ScoreMenu.h"
#include "editmenu.h"
#include "MainHBoxLayout.h"
#include <QStatusBar>
#include <QDockWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QWidget>
#include <QMainWindow>

class AbcMainWindow: public QMainWindow
{
	Q_OBJECT

public:
	AbcMainWindow(QWidget* parent = nullptr);
    ~AbcMainWindow();

    MainHBoxLayout *mainHBoxLayout();
    ScoreMenu *scoreMenu();

protected:
    void closeEvent(QCloseEvent *event);

private:
    ScoreMenu scoremenu;
    EditMenu editmenu;
	PreferencesMenu preferencesmenu;
    HelpMenu helpmenu;
    MainHBoxLayout mainhboxlayout;
};
#endif
