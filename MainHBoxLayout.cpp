// SPDX-License-Identifier: GPL-3.0-or-later

#include "MainHBoxLayout.h"

MainHBoxLayout::MainHBoxLayout(QWidget* parent)
	: QHBoxLayout(parent)
{
    addWidget(&edittabwidget);
    addWidget(&viewwidget);
    setStretchFactor(&edittabwidget, 2);
    setStretchFactor(&viewwidget, 1);
}

MainHBoxLayout::~MainHBoxLayout()
{
}

EditTabWidget *MainHBoxLayout::editTabWidget()
{
    return &edittabwidget;
}

ViewWidget *MainHBoxLayout::viewWidget()
{
    return &viewwidget;
}
