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
#ifndef SCOREMENU_H
#define SCOREMENU_H

#include <QMessageBox>
#include <QAction>
#include <QMenu>

class ScoreMenu: public QMenu
{
	Q_OBJECT

public:
	ScoreMenu(QWidget* parent = nullptr);
	~ScoreMenu();
	QMessageBox::StandardButton gracefulQuit();	

protected slots:
    void onQuitActionTriggered();
    void onOpenActionTriggered();
    void onSaveActionTriggered();
    void onSaveAsActionTriggered();
    void onCloseActionTriggered();
    void onNewActionTriggered();

private:
    QAction newaction;
    QAction openaction;
    QAction saveaction;
    QAction saveasaction;
    QAction closeaction;
    QAction quitaction;
};

#define NEW_TEMPLATE "X:1\nT:Melody...\nC:Piotr Ilitch Tchaïkovski\nM:4/4\nL:1/4\nK:Amin\n!mf!ABcd|e3c|e3A|cAFc|A4|]"
#endif
