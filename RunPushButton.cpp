#include "RunPushButton.h"

RunPushButton::RunPushButton(QWidget* parent)
	: QPushButton(parent)
{
    setText(tr("Generate score"));
}

RunPushButton::~RunPushButton()
{
}
