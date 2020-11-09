#include "CentralWidget.h"

CentralWidget::CentralWidget(QWidget* parent)
	: QWidget(parent)
{
	setLayout(&mainhboxlayout);
}

CentralWidget::~CentralWidget()
{
}

MainHBoxLayout *CentralWidget::mainHBoxLayout()
{
    return &mainhboxlayout;
}
