#include "PlayerAction.h"

PlayerAction::PlayerAction(QWidget* parent)
	: QAction(parent)
{
	setText(tr("Player"));
}

PlayerAction::~PlayerAction()
{
}
