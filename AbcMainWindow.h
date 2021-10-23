/*
 *   This file is part of QAbc.
 *
 *   QAbc is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   QAbc is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with QAbc.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef ABCMAINWINDOW_H
#define ABCMAINWINDOW_H

#include "HelpMenu.h"
#include "PreferencesMenu.h"
#include "ScoreMenu.h"
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

protected:
    void closeEvent(QCloseEvent *event);

private:
	ScoreMenu scoremenu;
	PreferencesMenu preferencesmenu;
    HelpMenu helpmenu;
    MainHBoxLayout mainhboxlayout;
};
#endif
