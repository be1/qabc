#include "OpenAction.h"

OpenAction::OpenAction(QWidget* parent)
	: QAction(parent)
{
	setText(tr("Open"));
}

OpenAction::~OpenAction()
{
}
