#include "PSPrefAction.h"

PSPrefAction::PSPrefAction(QWidget* parent)
	: QAction(parent)
{
    setText(tr("Score generator"));
}

PSPrefAction::~PSPrefAction()
{
}
