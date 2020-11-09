#include "ViewVBoxLayout.h"

ViewVBoxLayout::ViewVBoxLayout(QWidget* parent)
	: QVBoxLayout(parent)
{
    addWidget(&logview);
}

ViewVBoxLayout::~ViewVBoxLayout()
{
}

LogView *ViewVBoxLayout::logView()
{
   return &logview;
}
