#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "ViewVBoxLayout.h"
#include <QWidget>

class ViewWidget: public QWidget
{
	Q_OBJECT

public:
	ViewWidget(QWidget* parent = nullptr);
    ~ViewWidget();

    ViewVBoxLayout *viewVBoxLayout();

private:
	ViewVBoxLayout viewvboxlayout;
};
#endif
