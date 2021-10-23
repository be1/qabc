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
#include "ViewVBoxLayout.h"

ViewVBoxLayout::ViewVBoxLayout(QWidget* parent)
	: QVBoxLayout(parent)
{
    addWidget(&logview);
    clearbutton.setText(tr("Clear log"));
    addWidget(&clearbutton);
    connect(&clearbutton, &QPushButton::clicked, &logview, &QPlainTextEdit::clear);
}

ViewVBoxLayout::~ViewVBoxLayout()
{
}

LogView *ViewVBoxLayout::logView()
{
    return &logview;
}
