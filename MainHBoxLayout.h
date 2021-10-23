/*
 *   This file is part of QAbc.
 *
 *   QAbc is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   QAbc is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with QAbc.  If not, see <https://www.gnu.org/licenses/>.
 */
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
