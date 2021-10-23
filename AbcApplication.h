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
