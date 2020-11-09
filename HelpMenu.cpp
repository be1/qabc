#include "HelpMenu.h"
#include <QMessageBox>

HelpMenu::HelpMenu(QWidget* parent)
	: QMenu(parent)
{
	setTitle(tr("Help"));
	addAction(&aboutaction);
	connect(&aboutaction, SIGNAL(triggered()), this, SLOT(onAboutActionTriggered()));
	addAction(&aboutqtaction);
	connect(&aboutqtaction, SIGNAL(triggered()), this, SLOT(onAboutQtActionTriggered()));
}

HelpMenu::~HelpMenu()
{
}

void HelpMenu::onAboutActionTriggered()
{
    QMessageBox::about(this, tr("ABC score editor"), tr("Copyright © 2020 Benoît Rouits <brouits@free.fr>"));
}

void HelpMenu::onAboutQtActionTriggered()
{
    QMessageBox::aboutQt(this, tr("ABC score editor"));
}
