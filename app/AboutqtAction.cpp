#include "AboutqtAction.h"

AboutqtAction::AboutqtAction(QWidget* parent)
	: QAction(parent)
{
	setText(tr("About Qt"));
}

AboutqtAction::~AboutqtAction()
{
}
