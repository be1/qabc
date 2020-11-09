#include "SaveasAction.h"

SaveasAction::SaveasAction(QWidget* parent)
	: QAction(parent)
{
    setText(tr("Save as"));
}

SaveasAction::~SaveasAction()
{
}
