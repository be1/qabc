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
#include "config.h"
#include "AbcApplication.h"
#include "AbcMainWindow.h"
#include <QCommandLineParser>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char** argv)
{
	Q_INIT_RESOURCE(resources);

	AbcApplication abcapplication(argc, argv);

	QString locale = QLocale::system().name();
	QTranslator qtTranslator;
	qtTranslator.load("qt_" + locale,
			QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	abcapplication.installTranslator(&qtTranslator);

	QTranslator qabcTranslator;
	if (!qabcTranslator.load(TARGET "_" + locale, "locale"))
		qabcTranslator.load(TARGET "_" + locale, DATADIR "/" TARGET "/locale");
	abcapplication.installTranslator(&qabcTranslator);

	QCommandLineParser parser;
	parser.setApplicationDescription("ABC music notation minimal GUI.");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("score", QCoreApplication::translate("main", "ABC score file to work on."));

	parser.process(abcapplication);

	AbcMainWindow w;
	abcapplication.setMainWindow(&w);

	QString iconpath = QString(DATADIR "/pixmaps/" TARGET ".png");
	if (QFileInfo::exists(iconpath))
		abcapplication.setWindowIcon(QIcon(iconpath));

	const QStringList args = parser.positionalArguments();
	if (args.length() > 0)
		abcapplication.openFileNames(args);

	return abcapplication.exec();
}
