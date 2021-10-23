// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MAINHBOXLAYOUT_H
#define MAINHBOXLAYOUT_H

#include "ViewWidget.h"
#include "EditTabWidget.h"
#include <QHBoxLayout>

class MainHBoxLayout: public QHBoxLayout
{
	Q_OBJECT

public:
	MainHBoxLayout(QWidget* parent = nullptr);
	~MainHBoxLayout();

    EditTabWidget *editTabWidget();
    ViewWidget *viewWidget();

private:
	EditTabWidget edittabwidget;
	ViewWidget viewwidget;
};
#endif
