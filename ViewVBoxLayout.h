// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VIEWVBOXLAYOUT_H
#define VIEWVBOXLAYOUT_H

#include "RunPushButton.h"
#include "LogView.h"
#include <QVBoxLayout>

class ViewVBoxLayout: public QVBoxLayout
{
	Q_OBJECT

public:
	ViewVBoxLayout(QWidget* parent = nullptr);
    ~ViewVBoxLayout();

    LogView *logView();

private:
    LogView logview;
    QPushButton clearbutton;
};
#endif
