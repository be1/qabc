// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include "EditVBoxLayout.h"
#include <QWidget>
#include <QTabWidget>

class EditWidget: public QWidget
{
	Q_OBJECT

public:
    explicit EditWidget(const QString& fileName, QWidget* parent = nullptr);
    ~EditWidget();

    EditVBoxLayout *editVBoxLayout();
    QString *fileName();
    void setFileName(const QString& fileName);

private:
	EditVBoxLayout editvboxlayout;
    QString filename;
};

#endif
