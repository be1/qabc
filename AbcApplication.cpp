// SPDX-License-Identifier: GPL-3.0-or-later

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
