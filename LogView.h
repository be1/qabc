// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PDFWIDGET_H
#define PDFWIDGET_H

#include <QPlainTextEdit>

class LogView: public QPlainTextEdit
{
	Q_OBJECT

public:
    LogView(QWidget* parent = nullptr);
    ~LogView();

private:
};
#endif
