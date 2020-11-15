#include "ResetAction.h"

ResetAction::ResetAction(QWidget* parent)
	: QAction(parent)
{
    setText(tr("Reset"));
}

ResetAction::~ResetAction()
{
}
