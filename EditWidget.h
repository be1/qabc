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

class FileNameEditWidget : public EditWidget
{
    Q_OBJECT

public:
    FileNameEditWidget(const QString& fileName, QWidget *parent = nullptr)
        : EditWidget(fileName, parent),
          filename(fileName)
    {
    }

    QString *fileName();
    void setFileName(const QString& fileName);

private:
    QString filename;
};
#endif
