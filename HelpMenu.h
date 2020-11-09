#ifndef HELPMENU_H
#define HELPMENU_H

#include "AboutqtAction.h"
#include "AboutAction.h"
#include <QMenu>

class HelpMenu: public QMenu
{
	Q_OBJECT

public:
	HelpMenu(QWidget* parent = nullptr);
	~HelpMenu();

protected slots:
void onAboutActionTriggered();
void onAboutQtActionTriggered();

private:
	AboutAction aboutaction;
	AboutqtAction aboutqtaction;
};
#endif
