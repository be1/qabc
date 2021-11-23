// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AbcMainWindow.h"

AbcMainWindow::AbcMainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	setWindowTitle(tr("QAbc"));
	menuBar()->addMenu(&scoremenu);
	menuBar()->addMenu(&preferencesmenu);
    menuBar()->addMenu(&helpmenu);
    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(&mainhboxlayout);
    statusBar()->showMessage(tr("Idle"));
	setMinimumWidth(800);
	setMinimumHeight(480);
	show();
}

AbcMainWindow::~AbcMainWindow()
{
}

MainHBoxLayout *AbcMainWindow::mainHBoxLayout()
{
   return &mainhboxlayout;
}

void AbcMainWindow::closeEvent(QCloseEvent *event)
{
    if (QMessageBox::StandardButton::Yes == scoremenu.gracefulQuit()) {
        event->accept();
    } else {
        event->ignore();
    }
}
