#include "MainHBoxLayout.h"

MainHBoxLayout::MainHBoxLayout(QWidget* parent)
	: QHBoxLayout(parent)
{
	addWidget(&edittabwidget);
	addWidget(&viewwidget);
}

MainHBoxLayout::~MainHBoxLayout()
{
}

EditTabWidget *MainHBoxLayout::editTabWidget()
{
    return &edittabwidget;
}

ViewWidget *MainHBoxLayout::viewWidget()
{
    return &viewwidget;
}
