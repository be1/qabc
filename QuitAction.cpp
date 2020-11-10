#include "QuitAction.h"

QuitAction::QuitAction(QWidget* parent)
	: QAction(parent)
{
    setShortcut(QKeySequence(QKeySequence::Quit));
	setText(tr("Quit"));
}

QuitAction::~QuitAction()
{
}
