/*
 *   This file is part of QAbc.
 *
 *   QAbc is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   QAbc is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with QAbc.  If not, see <https://www.gnu.org/licenses/>.
 */
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
