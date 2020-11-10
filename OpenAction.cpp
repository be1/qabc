#include "OpenAction.h"

OpenAction::OpenAction(QWidget* parent)
	: QAction(parent)
{
    setShortcut(QKeySequence(QKeySequence::Open));
	setText(tr("Open"));
}

OpenAction::~OpenAction()
{
}
