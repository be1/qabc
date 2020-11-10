#include "PreferencesMenu.h"
#include <QDebug>
#include <QSettings>
#include <QInputDialog>

PreferencesMenu::PreferencesMenu(QWidget* parent)
    : QMenu(parent)
{
    setTitle(tr("Preferences"));


	addAction(&compileraction);
    addAction(&playeraction);
    addAction(&synthaction);
    vieweraction.setText(tr("Viewer"));
    addAction(&vieweraction);
    addAction(&resetaction);

    connect(&compileraction, &QAction::triggered, this, &PreferencesMenu::onCompilerActionTriggered);
    connect(&playeraction, &QAction::triggered, this, &PreferencesMenu::onPlayerActionTriggered);
    connect(&synthaction, &QAction::triggered, this, &PreferencesMenu::onSynthActionTriggered);
    connect(&resetaction, &QAction::triggered, this, &PreferencesMenu::onResetActionTriggered);
    connect(&vieweraction, &QAction::triggered, this, &PreferencesMenu::onViewerActionTriggered);
}

PreferencesMenu::~PreferencesMenu()
{
}

void PreferencesMenu::onCompilerActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant compiler = settings.value(COMPILER_KEY);

    bool ok;
    QString command;
    if (!compiler.isNull())
        command = QInputDialog::getText(this, tr("Compiler preference"), tr("Compiler:"), QLineEdit::Normal, compiler.toString(), &ok);
    else
        command = QInputDialog::getText(this, tr("Compiler preference"), tr("Compiler:"), QLineEdit::Normal, ABCM2PS, &ok);

    if (!ok)
        return;

    settings.setValue(COMPILER_KEY, command);
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
        command = QInputDialog::getText(this, tr("Player preference"), tr("Player:"), QLineEdit::Normal, ABC2MMIDI, &ok);

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

void PreferencesMenu::onViewerActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant viewer = settings.value(VIEWER_KEY);

    bool ok;
    QString command;
    if (!viewer.isNull())
        command = QInputDialog::getText(this, tr("PS viewer preference"), tr("PS Viewer:"), QLineEdit::Normal, viewer.toString(), &ok);
    else
        command = QInputDialog::getText(this, tr("PS viewer preference"), tr("PS Viewer:"), QLineEdit::Normal, PSVIEWER, &ok);

    if (!ok)
        return;

    settings.setValue(VIEWER_KEY, command);
    settings.sync();
}
void PreferencesMenu::onResetActionTriggered()
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);

    settings.setValue(COMPILER_KEY, ABCM2PS);

    settings.setValue(PLAYER_KEY, ABC2MMIDI);

    settings.setValue(SYNTH_KEY, FLUIDSYNTH);

    settings.setValue(VIEWER_KEY, PSVIEWER);

    settings.sync();
}
