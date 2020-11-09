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

QString *FileNameEditWidget::fileName()
{
    return &filename;
}

void FileNameEditWidget::setFileName(const QString &fileName)
{
    filename = fileName;
    editVBoxLayout()->setFileName(fileName);
}
