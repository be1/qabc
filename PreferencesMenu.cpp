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
    addAction(&resetaction);

    connect(&compileraction, &QAction::triggered, this, &PreferencesMenu::onCompilerActionTriggered);
    connect(&playeraction, &QAction::triggered, this, &PreferencesMenu::onPlayerActionTriggered);
    connect(&synthaction, &QAction::triggered, this, &PreferencesMenu::onSynthActionTriggered);
    connect(&resetaction, &QAction::triggered, this, &PreferencesMenu::onResetActionTriggered);
}

PreferencesMenu::~PreferencesMenu()
{
}

void PreferencesMenu::onCompilerActionTriggered()
{
    QSettings settings("Herewe", "QAbc");
    QVariant compiler = settings.value(COMPILER_KEY);

    bool ok;
    QString command;
    if (!compiler.isNull())
        command = QInputDialog::getText(this, tr("Compiler preference"), tr("Compiler:"), QLineEdit::Normal, compiler.toString(), &ok);
    else
        command = QInputDialog::getText(this, tr("Compiler preference"), tr("Compiler"), QLineEdit::Normal, ABCM2PS, &ok);

    if (!ok)
        return;

    settings.setValue(COMPILER_KEY, command);
    settings.sync();
}

void PreferencesMenu::onPlayerActionTriggered()
{
    QSettings settings("Herewe", "QAbc");
    QVariant player = settings.value(PLAYER_KEY);

    bool ok;
    QString command;
    if (!player.isNull())
        command = QInputDialog::getText(this, tr("Player preference"), tr("Player:"), QLineEdit::Normal, player.toString(), &ok);
    else
        command = QInputDialog::getText(this, tr("Player preference"), tr("Player"), QLineEdit::Normal, ABC2MMIDI, &ok);

    if (!ok)
        return;

    settings.setValue(PLAYER_KEY, command);
    settings.sync();
}

void PreferencesMenu::onSynthActionTriggered()
{
    QSettings settings("Herewe", "QAbc");
    QVariant synth = settings.value(SYNTH_KEY);

    bool ok;
    QString command;
    if (!synth.isNull())
        command = QInputDialog::getText(this, tr("Synth preference"), tr("Synth:"), QLineEdit::Normal, synth.toString(), &ok);
    else
        command = QInputDialog::getText(this, tr("Synth preference"), tr("Synth"), QLineEdit::Normal, FLUIDSYNTH, &ok);

    if (!ok)
        return;

    settings.setValue(SYNTH_KEY, command);
    settings.sync();
}

void PreferencesMenu::onResetActionTriggered()
{
    QSettings settings("Herewe", "QAbc");

    settings.setValue(COMPILER_KEY, ABCM2PS);

    settings.setValue(PLAYER_KEY, ABC2MMIDI);

    settings.setValue(SYNTH_KEY, FLUIDSYNTH);

    settings.sync();
}
