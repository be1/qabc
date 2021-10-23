#include "LogView.h"

LogView::LogView(QWidget* parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);
}

LogView::~LogView()
{
}
