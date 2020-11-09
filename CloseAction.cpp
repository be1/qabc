#include "CloseAction.h"

CloseAction::CloseAction(QWidget* parent)
	: QAction(parent)
{
	setText(tr("Close"));
}

CloseAction::~CloseAction()
{
}
