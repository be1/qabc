#include "AbcApplication.h"
#include <QSettings>
#include <QDebug>
#include "config.h"
#include "../abcm2ps/abcm2ps.h"

AbcApplication::AbcApplication(int& argc, char **argv)
	: QApplication(argc, argv)
{
    setOrganizationName(SETTINGS_DOMAIN);
	setOrganizationDomain("herewe");
    setApplicationName(SETTINGS_APP);
	setApplicationVersion(VERSION " (" REVISION ")");

    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);

    QVariant sfont = settings.value(SOUNDFONT_KEY);
    if (!sfont.isValid())
        settings.setValue(SOUNDFONT_KEY, DEB_SF2_GM);

    QVariant driver = settings.value(DRIVER_KEY);
    if (!sfont.isValid())
        settings.setValue(DRIVER_KEY, DRIVER_ALSA);

    QVariant player = settings.value(PLAYER_KEY);
	if (!player.isValid())
		settings.setValue(PLAYER_KEY, ABC2MIDI);
#ifndef USE_LIBABCM2PS
    QVariant compiler = settings.value(COMPILER_KEY);
	if (!compiler.isValid())
		settings.setValue(COMPILER_KEY, ABCM2PS);
#endif
    QVariant pstunes = settings.value(PSTUNES_KEY);
    if (!pstunes.isValid())
        settings.setValue(PSTUNES_KEY, TUNES_SELECTED);

    settings.sync();
	abcminit();
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
        EditTabWidget* edittabs = mainWindow()->mainHSplitter()->editTabWidget();
        for (int i = 0; i < fileNames.length(); i++) {
                QString fileName = fileNames[i];
                QFile file(fileName);
                if (file.open(QFile::ReadOnly | QFile::Text)) {
                        EditWidget* widget = new EditWidget(fileName, edittabs);
                        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
                        edit->setPlainText(file.readAll());
                        file.close();
                        edit->setSaved();
                        edittabs->addTab(widget);
                }
        }
}

bool AbcApplication::isquit = false;

void AbcApplication::quit()
{
    AbcApplication::isquit = true;
    QApplication::quit();
}

bool AbcApplication::isQuit()
{
    return AbcApplication::isquit;
}
