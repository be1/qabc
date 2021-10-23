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
#include "AbcApplication.h"
#include "settings.h"
#include <QDebug>
#include "config.h"

AbcApplication::AbcApplication(int& argc, char **argv)
	: QApplication(argc, argv)
{
    setOrganizationName(SETTINGS_DOMAIN);
	setOrganizationDomain("herewe");
    setApplicationName(SETTINGS_APP);
	setApplicationVersion(VERSION " (" REVISION ")");

    Settings settings;

    settings.check();

    settings.sync();
}

AbcApplication::~AbcApplication()
{
}

void AbcApplication::setMainWindow(AbcMainWindow* w)
{
	abcmainwindow = w;
}

AbcMainWindow *AbcApplication::mainWindow()
{
	return abcmainwindow;
}

void AbcApplication::openFileNames(const QStringList &fileNames)
{
    EditTabWidget* edittabs = mainWindow()->mainHBoxLayout()->editTabWidget();
	for (int i = 0; i < fileNames.length(); i++) {
		QString fileName = fileNames[i];
		QFile file(fileName);
		if (file.open(QFile::ReadOnly | QFile::Text)) {
			EditWidget* widget = new EditWidget(fileName, edittabs);
			edittabs->addTab(widget);
			AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
			edit->setPlainText(file.readAll());
			file.close();
		}
	}
}
