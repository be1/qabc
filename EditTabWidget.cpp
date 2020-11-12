#include "EditTabWidget.h"
#include <QFileInfo>
#include <QDebug>

EditTabWidget::EditTabWidget(QWidget* parent)
	: QTabWidget(parent)
{
    connect(this, &QTabWidget::currentChanged, this, &EditTabWidget::onCurrentChanged);
}

EditTabWidget::~EditTabWidget()
{
    for (int i = 0; i < editwidgetlist.length(); i++ ) {
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
    editwidgetlist.removeAt(index);
    QTabWidget::removeTab(index);
    delete w;
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
