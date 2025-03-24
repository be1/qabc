// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    if (qtTranslator.load(QLocale::system(), "qtbase", "_",
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
#else
    if (qtTranslator.load("qt_" + locale,
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
#endif
        abcapplication.installTranslator(&qtTranslator);

    QTranslator qabcTranslator;
    if (qabcTranslator.load(TARGET "_" + locale, "locale"))
        abcapplication.installTranslator(&qabcTranslator);
    else if (qabcTranslator.load(TARGET "_" + locale, DATADIR "/" TARGET "/locale"))
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
