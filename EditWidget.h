#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include "EditVBoxLayout.h"
#include <QWidget>

class EditWidget: public QWidget
{
	Q_OBJECT

public:
    explicit EditWidget(const QString& fileName, QWidget* parent = nullptr);
    ~EditWidget();

    EditVBoxLayout *editVBoxLayout();

private:
	EditVBoxLayout editvboxlayout;
};
#endif
