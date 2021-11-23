// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HELPMENU_H
#define HELPMENU_H

#include <QAction>
#include <QMenu>

class HelpMenu: public QMenu
{
	Q_OBJECT

public:
	HelpMenu(QWidget* parent = nullptr);
	~HelpMenu();

protected slots:
void onAboutActionTriggered();
void onAboutQtActionTriggered();

private:
    QAction aboutaction;
    QAction aboutqtaction;
};
#endif
