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
}

QList<FileNameEditWidget*> *EditTabWidget::fileNameEditWidgetList()
{
    return &filenameeditwidgetlist;
}

FileNameEditWidget *EditTabWidget::currentFileNameEditWidget()
{
    return currentfilenameeditwidget;
}

int EditTabWidget::addTab(FileNameEditWidget *swidget)
{
    QFileInfo info(*(swidget->fileName()));
    int ret = QTabWidget::addTab(swidget, info.baseName());
    setCurrentWidget(swidget);
    filenameeditwidgetlist.append(swidget);
    currentfilenameeditwidget = swidget;
    qDebug() << "addTab: " << (*currentfilenameeditwidget->fileName());
    return ret;
}

void EditTabWidget::removeTab(int index)
{
    filenameeditwidgetlist.removeAt(index);
    QTabWidget::removeTab(index);
}

void EditTabWidget::onCurrentChanged(int index)
{
    FileNameEditWidget* swidget = static_cast<FileNameEditWidget*>(currentWidget());
    for (int i = 0; i < filenameeditwidgetlist.length(); i++) {
        if (filenameeditwidgetlist.at(i) == swidget) {
            currentfilenameeditwidget = filenameeditwidgetlist.at(i);
            qDebug() << "currentTab: " << index << (*currentfilenameeditwidget->fileName()) << i;
            break;
        }
    }
}
