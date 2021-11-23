#include "ViewWidget.h"

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	setLayout(&viewvboxlayout);
}

ViewWidget::~ViewWidget()
{
}

ViewVBoxLayout *ViewWidget::viewVBoxLayout()
{
   return &viewvboxlayout;
}
