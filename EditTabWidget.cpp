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
    connect(this, &QTabWidget::currentChanged, this, &EditTabWidget::onCurrentChanged);
}

EditTabWidget::~EditTabWidget()
{
    for (int i = editwidgetlist.length() -1; i >= 0; i-- ) {
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
    editwidgetlist.append(swidget);
    currenteditwidget = swidget;
    qDebug() << "addTab: " << (*currenteditwidget->fileName());
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
    int len = editWidgetList()->length();
    for (int i = len -1; i >= 0; i-- ) {
        removeTab(i);
    }
}

void EditTabWidget::askRemoveTab(int index)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* m = a->mainWindow();

    EditWidget *w = editwidgetlist.at(index);
    if (!w->editVBoxLayout()->abcPlainTextEdit()->isSaved() &&
            (QMessageBox::StandardButton::No == QMessageBox::question(m, tr("Really close?"),
                                                                      tr("Current score not saved!\nClose this score anyway?"))))
        return;

    removeTab(index);
}

void EditTabWidget::onCurrentChanged(int index)
{
    EditWidget* swidget = static_cast<EditWidget*>(currentWidget());
    for (int i = 0; i < editwidgetlist.length(); i++) {
        if (editwidgetlist.at(i) == swidget) {
            currenteditwidget = editwidgetlist.at(i);
            qDebug() << "currentTab: " << index << (*currenteditwidget->fileName()) << i;
            break;
        }
    }
}
