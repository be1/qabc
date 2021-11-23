// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "EditWidget.h"
#include <QTabWidget>

EditWidget::EditWidget(const QString& fileName, QWidget *parent)
    : QWidget(parent),
      editvboxlayout(fileName, this),
      filename(fileName)
{
	setObjectName("EditWidget:" + fileName);
	setLayout(&editvboxlayout);
}

EditWidget::~EditWidget()
{
}

EditVBoxLayout *EditWidget::editVBoxLayout()
{
	return &editvboxlayout;
}

QString *EditWidget::fileName()
{
	return &filename;
}

void EditWidget::setFileName(const QString &fileName)
{
	filename = fileName;
	editVBoxLayout()->setFileName(fileName);
}
