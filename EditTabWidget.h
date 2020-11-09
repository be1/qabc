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

    QList<FileNameEditWidget*> *fileNameEditWidgetList();
    FileNameEditWidget *currentFileNameEditWidget();

    int addTab(FileNameEditWidget *swidget);
    void removeTab(int index);

protected slots:
    void onCurrentChanged(int index);

private:
    QList<FileNameEditWidget*> filenameeditwidgetlist;
    FileNameEditWidget* currentfilenameeditwidget;
};
#endif
