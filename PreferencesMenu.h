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
