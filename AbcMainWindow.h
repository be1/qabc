#ifndef ABCMAINWINDOW_H
#define ABCMAINWINDOW_H

#include "HelpMenu.h"
#include "PreferencesMenu.h"
#include "ScoreMenu.h"
#include "MainHBoxLayout.h"
#include <QStatusBar>
#include <QDockWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QWidget>
#include <QMainWindow>

class AbcMainWindow: public QMainWindow
{
	Q_OBJECT

public:
	AbcMainWindow(QWidget* parent = nullptr);
    ~AbcMainWindow();

    MainHBoxLayout *mainHBoxLayout();

private:
	ScoreMenu scoremenu;
	PreferencesMenu preferencesmenu;
    HelpMenu helpmenu;
    MainHBoxLayout mainhboxlayout;
};
#endif
