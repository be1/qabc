#include "config.h"
#include "AbcApplication.h"
#include "AbcMainWindow.h"
#include <QCommandLineParser>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char** argv)
{
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

	const QStringList args = parser.positionalArguments();
	if (args.length() > 0)
		abcapplication.openFileNames(args);

	QString iconpath = QString(DATADIR "/pixmaps/" TARGET ".png");
	if (QFileInfo::exists(iconpath))
		abcapplication.setWindowIcon(QIcon(iconpath));

	AbcMainWindow w;
	abcapplication.setMainWindow(&w);
	return abcapplication.exec();
}
