#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include "MainHBoxLayout.h"
#include <QWidget>

class CentralWidget: public QWidget
{
	Q_OBJECT

public:
	CentralWidget(QWidget* parent = nullptr);
	~CentralWidget();

    MainHBoxLayout *mainHBoxLayout();

private:
	MainHBoxLayout mainhboxlayout;
};
#endif
