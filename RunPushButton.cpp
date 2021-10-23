// SPDX-License-Identifier: GPL-3.0-or-later

#include "RunPushButton.h"

RunPushButton::RunPushButton(QWidget* parent)
	: QPushButton(parent)
{
    setText(tr("&View score"));
    setIcon(QIcon::fromTheme("document-print-preview"));
}

RunPushButton::~RunPushButton()
{
}
