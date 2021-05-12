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


    addAction(&playeraction);
    addAction(&adriveraction);
    addAction(&sfontaction);
    psaction.setText(tr("Postscript export"));
    addAction(&psaction);
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

    bool ok;
    QString drv;
    if (!driver.isNull())
        drv = QInputDialog::getText(a->mainWindow(), tr("Audio driver preference"), tr("Audio driver:"), QLineEdit::Normal, driver.toString(), &ok);
    else
        drv = QInputDialog::getText(a->mainWindow(), tr("Audio driver preference"), tr("Audio driver:"), QLineEdit::Normal, ALSA, &ok);

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

    bool ok;
    QString command;
    if (!player.isNull())
        command = QInputDialog::getText(a->mainWindow(), tr("Player preference"), tr("Player:"), QLineEdit::Normal, player.toString(), &ok);
    else
        command = QInputDialog::getText(a->mainWindow(), tr("Player preference"), tr("Player:"), QLineEdit::Normal, ABC2MIDI, &ok);

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

    bool ok;
    QString param;
    if (!ps.isNull())
        param = QInputDialog::getText(a->mainWindow(), tr("Postscript export preference"), tr("Tunes:"), QLineEdit::Normal, ps.toString(), &ok);
    else
        param = QInputDialog::getText(a->mainWindow(), tr("Postscript export preference"), tr("Tunes:"), QLineEdit::Normal, TUNES_SELECTED, &ok);

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

    settings.setValue(DRIVER_KEY, ALSA);

    settings.setValue(SOUNDFONT_KEY, DEB_SF2_GM);

    settings.setValue(PSTUNES_KEY, TUNES_SELECTED);

    settings.sync();
}
