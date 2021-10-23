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
#ifndef EDITTABWIDGET_H
#define EDITTABWIDGET_H

#include "EditWidget.h"
#include <QTabWidget>



class EditTabWidget: public QTabWidget
{
	Q_OBJECT

public:
	EditTabWidget(QWidget* parent = nullptr);
    ~EditTabWidget();

    QList<EditWidget*> *editWidgetList();
    EditWidget *currentEditWidget();

    int addTab(EditWidget *swidget);
    void removeTab(int index);
    void removeTabs(void);
    void askRemoveTab(int index);

protected slots:
    void onCurrentChanged(int index);

private:
    QList<EditWidget*> editwidgetlist;
    EditWidget* currenteditwidget;
};
#endif
