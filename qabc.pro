QT += core widgets gui
TEMPLATE = app
TARGET = qabc
RESOURCES += resources.qrc
DISTFILES +=  dict.txt gm.txt
SOURCES = ScoreMenu.cpp PreferencesMenu.cpp HelpMenu.cpp AbcPlainTextEdit.cpp PlayPushButton.cpp EditVBoxLayout.cpp EditWidget.cpp EditTabWidget.cpp RunPushButton.cpp ViewVBoxLayout.cpp ViewWidget.cpp MainHBoxLayout.cpp AbcMainWindow.cpp AbcApplication.cpp main.cpp \
	AbcProcess.cpp \
	AbcTemporaryFile.cpp \
	LogView.cpp \
	editorprefdialog.cpp \
	settings.cpp
HEADERS = ScoreMenu.h PreferencesMenu.h HelpMenu.h AbcPlainTextEdit.h PlayPushButton.h EditVBoxLayout.h EditWidget.h EditTabWidget.h RunPushButton.h ViewVBoxLayout.h ViewWidget.h MainHBoxLayout.h AbcMainWindow.h AbcApplication.h \
	AbcProcess.h \
	AbcTemporaryFile.h \
	LogView.h \
	editorprefdialog.h \
	settings.h
VERSION = 1.8
REVISION = $$system(git describe --long --tags 2>/dev/null || echo "stable")
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
isEmpty(PREFIX): PREFIX = /usr/local
isEmpty(BINDIR): BINDIR = $$PREFIX/bin
isEmpty(DATADIR): DATADIR = $$PREFIX/share
config.input = config.h.in
config.output = config.h
QMAKE_SUBSTITUTES += config
isEmpty(QMAKE_LRELEASE):QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
TRANSLATIONS += $${TARGET}_en.ts $${TARGET}_fr.ts
LOCALE_DIR = locale
updateqm.input = TRANSLATIONS
updateqm.output = $$LOCALE_DIR/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $$LOCALE_DIR/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm
target.path = $$BINDIR
translations.path = $$DATADIR/$$TARGET
translations.files = $$LOCALE_DIR
desktop.path = $$DATADIR/applications
desktop.files = $${TARGET}.desktop
icon.path = $$DATADIR/pixmaps
icon.files = $${TARGET}.png
mime.path = $$DATADIR/mime/packages
mime.files = application-vnd-abc.xml
INSTALLS += target translations desktop icon mime
