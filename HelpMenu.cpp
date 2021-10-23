// SPDX-License-Identifier: GPL-3.0-or-later

#include "AbcApplication.h"
#include "HelpMenu.h"
#include "config.h"
#include <QMessageBox>

HelpMenu::HelpMenu(QWidget* parent)
	: QMenu(parent)
{
    setTitle(tr("Help"));

    aboutaction.setText(tr("About"));
    addAction(&aboutaction);

    aboutqtaction.setText(tr("About Qt"));
    addAction(&aboutqtaction);

    connect(&aboutaction, SIGNAL(triggered()), this, SLOT(onAboutActionTriggered()));
	connect(&aboutqtaction, SIGNAL(triggered()), this, SLOT(onAboutQtActionTriggered()));
}

HelpMenu::~HelpMenu()
{
}

void HelpMenu::onAboutActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    QMessageBox::about(a->mainWindow(), tr("ABC score editor"), tr("\nQAbc version ") + VERSION + " (" + REVISION + ")\n" +tr( "Copyright © 2020 Benoît Rouits <brouits@free.fr>"));
}

void HelpMenu::onAboutQtActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    QMessageBox::aboutQt(a->mainWindow(), tr("ABC score editor"));
}
