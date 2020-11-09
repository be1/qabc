QT += core widgets gui
TEMPLATE = app
TARGET = qabc
SOURCES = NewAction.cpp OpenAction.cpp SaveAction.cpp SaveasAction.cpp CloseAction.cpp QuitAction.cpp ScoreMenu.cpp CompilerAction.cpp PlayerAction.cpp PreferencesMenu.cpp AboutAction.cpp AboutqtAction.cpp HelpMenu.cpp AbcPlainTextEdit.cpp PlayPushButton.cpp EditVBoxLayout.cpp EditWidget.cpp EditTabWidget.cpp RunPushButton.cpp ViewVBoxLayout.cpp ViewWidget.cpp MainHBoxLayout.cpp CentralWidget.cpp AbcMainWindow.cpp AbcApplication.cpp main.cpp \
	AbcProcess.cpp \
	LogView.cpp \
	ResetAction.cpp \
	SynthAction.cpp
HEADERS = NewAction.h OpenAction.h SaveAction.h SaveasAction.h CloseAction.h QuitAction.h ScoreMenu.h CompilerAction.h PlayerAction.h PreferencesMenu.h AboutAction.h AboutqtAction.h HelpMenu.h AbcPlainTextEdit.h PlayPushButton.h EditVBoxLayout.h EditWidget.h EditTabWidget.h RunPushButton.h ViewVBoxLayout.h ViewWidget.h MainHBoxLayout.h CentralWidget.h AbcMainWindow.h AbcApplication.h \
	AbcProcess.h \
	LogView.h \
	ResetAction.h \
	SynthAction.h
