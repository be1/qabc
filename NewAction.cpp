#include "NewAction.h"

NewAction::NewAction(QWidget* parent)
	: QAction(parent)
{
	setText(tr("New"));
}

NewAction::~NewAction()
{
}
