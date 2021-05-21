#include "PreferencesMenu.h"
#include <QDebug>
#include <QSettings>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "AbcApplication.h"

PreferencesMenu::PreferencesMenu(QWidget* parent)
    : QMenu(parent)
{
    setTitle(tr("Preferences"));

    playeraction.setText(tr("MIDI Generator"));
    addAction(&playeraction);

    adriveraction.setText(tr("Audio output driver"));
    addAction(&adriveraction);

    sfontaction.setText(tr("Audio sound font"));
    addAction(&sfontaction);

    psaction.setText(tr("Postscript export"));
    addAction(&psaction);

    resetaction.setText(tr("Reset settings"));
    addAction(&resetaction);

    connect(&playeraction, &QAction::triggered, this, &PreferencesMenu::onPlayerActionTriggered);
    connect(&adriveraction, &QAction::triggered, this, &PreferencesMenu::onAdriverActionTriggered);
    connect(&sfontaction, &QAction::triggered, this, &PreferencesMenu::onSfontActionTriggered);
    connect(&psaction, &QAction::triggered, this, &PreferencesMenu::onPsActionTriggered);
    connect(&resetaction, &QAction::triggered, this, &PreferencesMenu::onResetActionTriggered);
}

PreferencesMenu::~PreferencesMenu()
{
}

void PreferencesMenu::onAdriverActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant driver = settings.value(DRIVER_KEY);

    AbcApplication* a = static_cast<AbcApplication*>(qApp);

    QStringList items;
    if (!driver.isNull()) {
        items << driver.toString();
    }

    items << DRIVER_ALSA << DRIVER_PULSEAUDIO << DRIVER_JACK << DRIVER_PORTAUDIO;
    items.removeDuplicates();

    bool ok;
    QString drv;
    drv = QInputDialog::getItem(a->mainWindow(), tr("Audio driver preference"), tr("Audio driver:"), items, 0, false, &ok);

    if (!ok)
        return;

    settings.setValue(DRIVER_KEY, drv);
    settings.sync();
}

void PreferencesMenu::onPlayerActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant player = settings.value(PLAYER_KEY);

    AbcApplication* a = static_cast<AbcApplication*>(qApp);

    QStringList items;
    if (!player.isNull()) {
        items << player.toString();
    }

    items << ABC2MIDI << LIBABC2SMF;
    items.removeDuplicates();

    bool ok;
    QString command;
    command = QInputDialog::getItem(a->mainWindow(), tr("MIDI Generator preference"), tr("MIDI generator:"), items, 0, true, &ok);

    if (!ok)
        return;

    settings.setValue(PLAYER_KEY, command);
    settings.sync();
}

void PreferencesMenu::onSfontActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant soundfont = settings.value(SOUNDFONT_KEY);

    AbcApplication* a = static_cast<AbcApplication*>(qApp);

    QString sf;
    if (!soundfont.isNull()) {
        QFileInfo info(soundfont.toString());
        sf = QFileDialog::getOpenFileName(a->mainWindow(), tr("Audio sound font preference"), info.absolutePath(), tr("Soundfont (*.sf[23])"));
    } else {
        sf = QFileDialog::getOpenFileName(a->mainWindow(), tr("Audio sound font preference"), QDir::homePath(), tr("Soundfont (*.sf[23])"));
    }

    if (sf.isNull())
        return;

    settings.setValue(SOUNDFONT_KEY, sf);
    settings.sync();
}

void PreferencesMenu::onPsActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant ps = settings.value(PSTUNES_KEY);

    AbcApplication* a = static_cast<AbcApplication*>(qApp);

    QStringList items;
    if (!ps.isNull()) {
        items << ps.toString();
    }

    items << TUNES_SELECTED << TUNES_ALL;
    items.removeDuplicates();

    bool ok;
    QString param;
    param = QInputDialog::getItem(a->mainWindow(), tr("Postscript export preference"), tr("Tunes:"), items, 0, false, &ok);

    if (!ok)
        return;

    settings.setValue(PSTUNES_KEY, param);
    settings.sync();

}

void PreferencesMenu::onResetActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);

    if (QMessageBox::No == QMessageBox::question(a->mainWindow(), tr("Reset prefrences?"), tr("Do you really want to reset preferences?")))
        return;

    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
#ifndef USE_LIBABCM2PS
	settings.setValue(COMPILER_KEY, ABCM2PS);
#endif
    settings.setValue(PLAYER_KEY, ABC2MIDI);

    settings.setValue(DRIVER_KEY, DRIVER_ALSA);

    settings.setValue(SOUNDFONT_KEY, DEB_SF2_GM);

    settings.setValue(PSTUNES_KEY, TUNES_SELECTED);

    settings.sync();
}
