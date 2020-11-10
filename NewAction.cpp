#include "NewAction.h"

NewAction::NewAction(QWidget* parent)
	: QAction(parent)
{
    setShortcut(QKeySequence(QKeySequence::New));
	setText(tr("New"));
}

NewAction::~NewAction()
{
}
