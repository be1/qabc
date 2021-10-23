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
