// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
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
