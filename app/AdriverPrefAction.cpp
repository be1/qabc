#include "AdriverPrefAction.h"

AdriverPrefAction::AdriverPrefAction(QWidget* parent)
	: QAction(parent)
{
    setText(tr("Audio output driver"));
}

AdriverPrefAction::~AdriverPrefAction()
{
}
