#include "SaveAction.h"

SaveAction::SaveAction(QWidget* parent)
	: QAction(parent)
{
	setText(tr("Save"));
}

SaveAction::~SaveAction()
{
}
