#include "SaveAction.h"

SaveAction::SaveAction(QWidget* parent)
	: QAction(parent)
{
    setShortcut(QKeySequence(QKeySequence::Save));
	setText(tr("Save"));
}

SaveAction::~SaveAction()
{
}
