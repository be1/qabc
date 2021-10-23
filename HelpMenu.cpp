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
