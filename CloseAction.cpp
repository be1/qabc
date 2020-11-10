#include "CloseAction.h"

CloseAction::CloseAction(QWidget* parent)
	: QAction(parent)
{
    setShortcut(QKeySequence(QKeySequence::Close));
	setText(tr("Close"));
}

CloseAction::~CloseAction()
{
}
