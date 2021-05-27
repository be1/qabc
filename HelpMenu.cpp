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
    QMessageBox::about(this, tr("ABC score editor"), tr("\nQAbc version ") + VERSION + " (" + REVISION + ")\n" +tr( "Copyright © 2020 Benoît Rouits <brouits@free.fr>"));
}

void HelpMenu::onAboutQtActionTriggered()
{
    QMessageBox::aboutQt(this, tr("ABC score editor"));
}
