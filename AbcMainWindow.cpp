#include "AbcMainWindow.h"

AbcMainWindow::AbcMainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	setWindowTitle(tr("QAbc"));
	menuBar()->addMenu(&scoremenu);
	menuBar()->addMenu(&preferencesmenu);
	menuBar()->addMenu(&helpmenu);
	setCentralWidget(&centralwidget);
	setMinimumWidth(800);
	setMinimumHeight(480);
	show();
}

AbcMainWindow::~AbcMainWindow()
{
}
