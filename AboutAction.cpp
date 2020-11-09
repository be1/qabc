#include "AboutAction.h"

AboutAction::AboutAction(QWidget* parent)
	: QAction(parent)
{
	setText(tr("About"));
}

AboutAction::~AboutAction()
{
}
