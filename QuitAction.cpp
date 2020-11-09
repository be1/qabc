#include "QuitAction.h"

QuitAction::QuitAction(QWidget* parent)
	: QAction(parent)
{
	setText(tr("Quit"));
}

QuitAction::~QuitAction()
{
}
