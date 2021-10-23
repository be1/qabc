// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ABCAPPLICATION_H
#define ABCAPPLICATION_H

#include "AbcMainWindow.h"
#include "AbcProcess.h"
#include <QApplication>

class AbcApplication: public QApplication
{
	Q_OBJECT

public:
	AbcApplication(int& argc, char **argv);
	~AbcApplication();

	void setMainWindow(AbcMainWindow* w);
	AbcMainWindow *mainWindow();
	void openFileNames(const QStringList& fileNames);
private:
	AbcMainWindow *abcmainwindow;
};
#endif
