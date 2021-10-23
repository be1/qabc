// SPDX-License-Identifier: GPL-3.0-or-later

#include "LogView.h"

LogView::LogView(QWidget* parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);
}

LogView::~LogView()
{
}
