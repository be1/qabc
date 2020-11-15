#include "AbcMainWindow.h"

AbcMainWindow::AbcMainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	setWindowTitle(tr("QAbc"));
	menuBar()->addMenu(&scoremenu);
	menuBar()->addMenu(&preferencesmenu);
    menuBar()->addMenu(&helpmenu);
    setCentralWidget(&mainhsplitter);
    statusBar()->showMessage(tr("Idle"));
    setMinimumWidth(1280);
    setMinimumHeight(720);
	show();
}

AbcMainWindow::~AbcMainWindow()
{
}

MainHSplitter *AbcMainWindow::mainHSplitter()
{
   return &mainhsplitter;
}
