// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    bool loadFile(const QString& fileName);

protected:
    void setRecentFile(const QString& fileName, bool ok);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

protected slots:
    void onQuitActionTriggered();
    void onOpenActionTriggered();
    void onOpenRecentActionTriggered();
    void onSaveActionTriggered();
    void onSaveAsActionTriggered();
    void onCloseActionTriggered();
    void onNewActionTriggered();

private:
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
};

#define NEW_TEMPLATE "X:1\nT:Melody...\nC:Piotr Ilitch Tchaïkovski\nM:4/4\nL:1/4\nK:Amin\n!mf!ABcd|e3c|e3A|cAFc|A4|]"
#endif
