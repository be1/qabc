// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
