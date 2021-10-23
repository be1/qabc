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
