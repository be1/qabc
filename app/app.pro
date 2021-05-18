include(../common.pri)
QT += core widgets gui svg printsupport
TEMPLATE = app
CONFIG += link_pkgconfig
PKGCONFIG += fluidsynth drumstick-file
DEFINES += USE_LIBABCM2PS _FORTIFY_SOURCE=2
QMAKE_CXXFLAGS = -O2
TARGET = qabc
RESOURCES += resources.qrc
DISTFILES +=  dict.txt config.h.in
SOURCES = NewAction.cpp OpenAction.cpp SaveAction.cpp SaveasAction.cpp CloseAction.cpp QuitAction.cpp ScoreMenu.cpp PreferencesMenu.cpp AboutAction.cpp AboutqtAction.cpp HelpMenu.cpp AbcPlainTextEdit.cpp PlayPushButton.cpp EditVBoxLayout.cpp EditWidget.cpp EditTabWidget.cpp RunPushButton.cpp AbcMainWindow.cpp AbcApplication.cpp main.cpp \
	AbcProcess.cpp \
	AbcTemporaryFile.cpp \
	AdriverPrefAction.cpp \
	LogView.cpp \
	MIDIPrefAction.cpp \
	MainHSplitter.cpp \
	ResetAction.cpp \
	ScoreSvgWidget.cpp \
	SfontPrefAction.cpp \
	TuneWaiter.cpp \
	ViewVSplitter.cpp \
	abcsmf.cpp \
	abcsynth.cpp \
	generator.cpp \
	midigenerator.cpp \
	psgenerator.cpp \
	sfloader.cpp \
	QProgressIndicator.cpp \
	svggenerator.cpp
HEADERS = NewAction.h OpenAction.h SaveAction.h SaveasAction.h CloseAction.h QuitAction.h ScoreMenu.h PreferencesMenu.h AboutAction.h AboutqtAction.h HelpMenu.h AbcPlainTextEdit.h PlayPushButton.h EditVBoxLayout.h EditWidget.h EditTabWidget.h RunPushButton.h AbcMainWindow.h AbcApplication.h \
	AbcProcess.h \
	AbcTemporaryFile.h \
	AdriverPrefAction.h \
	LogView.h \
	MIDIPrefAction.h \
	MainHSplitter.h \
	ResetAction.h \
	ScoreSvgWidget.h \
	SfontPrefAction.h \
	TuneWaiter.h \
	ViewVSplitter.h \
	abcsmf.h \
	abcsynth.h \
	config.h \
	generator.h \
	midigenerator.h \
	psgenerator.h \
	sfloader.h \
	QProgressIndicator.h \
	svggenerator.h
config.input = config.h.in
config.output = config.h
QMAKE_SUBSTITUTES += config
LIBS += ../abcm2ps/libabcm2ps.a ../abc/libabc.a
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
desktop.files = qabc.desktop
icon.path = $$DATADIR/icons/hicolor/scalable/apps
icon.files = qabc.svg
mime.path = $$DATADIR/mime/packages
mime.files = application-vnd-abc.xml
INSTALLS += target translations desktop icon mime

