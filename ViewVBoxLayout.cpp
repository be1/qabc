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
