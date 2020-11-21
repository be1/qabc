#include "PreferencesMenu.h"
#include <QDebug>
#include <QSettings>
#include <QInputDialog>

PreferencesMenu::PreferencesMenu(QWidget* parent)
    : QMenu(parent)
{
    setTitle(tr("Preferences"));


    addAction(&playeraction);
    addAction(&adriveraction);
    addAction(&sfontaction);
    addAction(&resetaction);

    connect(&playeraction, &QAction::triggered, this, &PreferencesMenu::onPlayerActionTriggered);
    connect(&adriveraction, &QAction::triggered, this, &PreferencesMenu::onAdriverActionTriggered);
    connect(&sfontaction, &QAction::triggered, this, &PreferencesMenu::onSfontActionTriggered);
    connect(&resetaction, &QAction::triggered, this, &PreferencesMenu::onResetActionTriggered);
}

PreferencesMenu::~PreferencesMenu()
{
}

void PreferencesMenu::onAdriverActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant driver = settings.value(DRIVER_KEY);

    bool ok;
    QString drv;
    if (!driver.isNull())
        drv = QInputDialog::getText(this, tr("Audio driver preference"), tr("Audio driver:"), QLineEdit::Normal, driver.toString(), &ok);
    else
        drv = QInputDialog::getText(this, tr("Audio driver preference"), tr("Audio driver:"), QLineEdit::Normal, ALSA, &ok);

    if (!ok)
        return;

    settings.setValue(DRIVER_KEY, drv);
    settings.sync();
}

void PreferencesMenu::onPlayerActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant player = settings.value(PLAYER_KEY);

    bool ok;
    QString command;
    if (!player.isNull())
        command = QInputDialog::getText(this, tr("Player preference"), tr("Player:"), QLineEdit::Normal, player.toString(), &ok);
    else
        command = QInputDialog::getText(this, tr("Player preference"), tr("Player:"), QLineEdit::Normal, ABC2MIDI, &ok);

    if (!ok)
        return;

    settings.setValue(PLAYER_KEY, command);
    settings.sync();
}

void PreferencesMenu::onSynthActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant synth = settings.value(SYNTH_KEY);

    bool ok;
    QString command;
    if (!synth.isNull())
        command = QInputDialog::getText(this, tr("Synth preference"), tr("Synth:"), QLineEdit::Normal, synth.toString(), &ok);
    else
        command = QInputDialog::getText(this, tr("Synth preference"), tr("Synth:"), QLineEdit::Normal, FLUIDSYNTH, &ok);

    if (!ok)
        return;

    settings.setValue(SYNTH_KEY, command);
    settings.sync();
}

void PreferencesMenu::onSfontActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant soundfont = settings.value(SOUNDFONT_KEY);

    bool ok;
    QString sf;
    if (!soundfont.isNull())
        sf = QInputDialog::getText(this, tr("Audio sound font preference"), tr("Soundfont:"), QLineEdit::Normal, soundfont.toString(), &ok);
    else
        sf = QInputDialog::getText(this, tr("Audio sound font preference"), tr("Soundfont:"), QLineEdit::Normal, DEB_SF2_GM, &ok);

    if (!ok)
        return;

    settings.setValue(SOUNDFONT_KEY, sf);
    settings.sync();
}

void PreferencesMenu::onResetActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);

    settings.setValue(PLAYER_KEY, ABC2MIDI);

    settings.setValue(SYNTH_KEY, FLUIDSYNTH);

    settings.setValue(DRIVER_KEY, ALSA);

    settings.setValue(SOUNDFONT_KEY, DEB_SF2_GM);

    settings.sync();
}
