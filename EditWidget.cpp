#include "EditWidget.h"

EditWidget::EditWidget(const QString& fileName, QWidget* parent)
    : QWidget(parent),
      editvboxlayout(fileName, this)
{
	setLayout(&editvboxlayout);
}

EditWidget::~EditWidget()
{
}

EditVBoxLayout *EditWidget::editVBoxLayout()
{
   return &editvboxlayout;
}
