// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RUNPUSHBUTTON_H
#define RUNPUSHBUTTON_H

#include <QPushButton>

class RunPushButton: public QPushButton
{
	Q_OBJECT

public:
	RunPushButton(QWidget* parent = nullptr);
	~RunPushButton();

private:
};
#endif
