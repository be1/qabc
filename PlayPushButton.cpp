#include "PlayPushButton.h"

PlayPushButton::PlayPushButton(QWidget* parent)
	: QPushButton(parent)
{
	setText(tr("Play"));
}

PlayPushButton::~PlayPushButton()
{
}
