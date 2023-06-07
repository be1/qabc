// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AbcApplication.h"
#include "EditTabWidget.h"
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>

EditTabWidget::EditTabWidget(QWidget* parent)
	: QTabWidget(parent)
{
    setTabsClosable(true);
    connect(this, &QTabWidget::tabCloseRequested, this, &EditTabWidget::askRemoveTab);
    connect(this, &QTabWidget::currentChanged, this, &EditTabWidget::onCurrentTabChanged);
}

EditTabWidget::~EditTabWidget()
{
    for (int i = count() -1; i >= 0; i-- ) {
        removeTab(i);
    }
}

QList<EditWidget*> *EditTabWidget::editWidgetList()
{
    return &editwidgetlist;
}

EditWidget *EditTabWidget::currentEditWidget()
{
    return currenteditwidget;
}

int EditTabWidget::addTab(EditWidget *swidget)
{
    QFileInfo info(*(swidget->fileName()));
    int ret = QTabWidget::addTab(swidget, info.baseName());
    setCurrentWidget(swidget);
    connect(swidget->editVBoxLayout()->abcPlainTextEdit(), &QPlainTextEdit::modificationChanged,
            this, &EditTabWidget::onCurrentTextModified);
    qDebug() << "addTab: " << (*swidget->fileName());
    editwidgetlist.append(swidget);
    currenteditwidget = swidget;
    return ret;
}

void EditTabWidget::removeTab(int index)
{
    EditWidget *w = editwidgetlist.at(index);
    w->editVBoxLayout()->cleanup();
    editwidgetlist.removeAt(index);
    QTabWidget::removeTab(index);
    delete w;
}

void EditTabWidget::removeTabs()
{
    for (int i = count() -1; i >= 0; i-- ) {
        removeTab(i);
    }
}

void EditTabWidget::askRemoveTab(int index)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* m = a->mainWindow();

    EditWidget *w = qobject_cast<EditWidget*>(widget(index));
    if (!w->editVBoxLayout()->abcPlainTextEdit()->isSaved() &&
            (QMessageBox::StandardButton::No == QMessageBox::question(m, tr("Really close?"),
                                                                      tr("Current score not saved!\nClose this score anyway?"))))
        return;

    removeTab(index);
}

void EditTabWidget::onCurrentTabChanged(int index)
{
    if (index < 0)
        return;

    EditWidget* swidget = static_cast<EditWidget*>(currentWidget());
    for (int i = 0; i < editwidgetlist.length(); i++) {
        if (editwidgetlist.at(i) == swidget) {
            currenteditwidget = editwidgetlist.at(i);
            qDebug() << "currentTab: " << index << (*currenteditwidget->fileName()) << i;
            break;
        }
    }
}

void EditTabWidget::onCurrentTextModified(bool modified)
{
    int ci = currentIndex();
    QString text = tabText(ci);

    if (modified) {
        if (!text.endsWith('*'))
            setTabText(ci, text + "*");
    } else {
        if (text.endsWith("*")) {
            text.chop(1);
            setTabText(ci, text);
        }
    }
}
