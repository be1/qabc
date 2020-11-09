#include "AbcApplication.h"
#include <QSettings>
#include <QDebug>
#include "config.h"

AbcApplication::AbcApplication(int& argc, char **argv)
	: QApplication(argc, argv)
{
	setOrganizationName("Herewe");
	setOrganizationDomain("herewe");
    setApplicationName("QAbc");
    setApplicationVersion(VERSION " (" REVISION ")");

    QSettings settings("Herewe", "QAbc");

    QVariant compiler = settings.value(COMPILER_KEY);
    if (!compiler.isValid())
        settings.setValue(COMPILER_KEY, ABCM2PS);

    QVariant player = settings.value(PLAYER_KEY);
    if (!player.isValid())
        settings.setValue(PLAYER_KEY, ABC2MMIDI);

    QVariant synth = settings.value(SYNTH_KEY);
    if (!synth.isValid())
        settings.setValue(SYNTH_KEY, FLUIDSYNTH);

    settings.sync();
}

AbcApplication::~AbcApplication()
{
}

AbcMainWindow *AbcApplication::mainWindow()
{
    return &abcmainwindow;
}

void AbcApplication::openFileNames(const QStringList &fileNames)
{
    CentralWidget *cw = static_cast<CentralWidget*>(mainWindow()->centralWidget());
    EditTabWidget* edittabs = cw->mainHBoxLayout()->editTabWidget();
    for (int i = 0; i < fileNames.length(); i++) {
        QString fileName = fileNames[i];
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            EditWidget* widget = new EditWidget(fileName, edittabs);
            edittabs->addTab(widget);
            AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
            edit->setPlainText(file.readAll());
            file.close();
        }
    }
}
