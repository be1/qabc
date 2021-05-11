#include "ScoreMenu.h"
#include "AbcApplication.h"
#include "AbcPlainTextEdit.h"
#include "EditTabWidget.h"
#include "EditWidget.h"
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

ScoreMenu::ScoreMenu(QWidget* parent)
	: QMenu(parent)
{
	setTitle(tr("Score"));
	addAction(&newaction);
	addAction(&openaction);
	addAction(&saveaction);
    addAction(&saveasaction);
    exportaction.setText(tr("Export to MIDI"));
    addAction(&exportaction);
    addAction(&closeaction);
	addAction(&quitaction);

	connect(&quitaction, SIGNAL(triggered()), this, SLOT(onQuitActionTriggered()));
    connect(&openaction, SIGNAL(triggered()), this, SLOT(onOpenActionTriggered()));
    connect(&saveaction, SIGNAL(triggered()), this, SLOT(onSaveActionTriggered()));
    connect(&saveasaction, SIGNAL(triggered()), this, SLOT(onSaveAsActionTriggered()));
    connect(&closeaction, SIGNAL(triggered()), this, SLOT(onCloseActionTriggered()));
    connect(&newaction, SIGNAL(triggered()), this, SLOT(onNewActionTriggered()));
    connect(&exportaction, SIGNAL(triggered()), this, SLOT(onExportActionTriggered()));
}

ScoreMenu::~ScoreMenu()
{
}

void ScoreMenu::onQuitActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    if (QMessageBox::StandardButton::No == QMessageBox::question(a->mainWindow(), tr("Really quit?"), tr("Do you really want to quit?")))
        return;

    EditTabWidget* tabs = a->mainWindow()->mainHSplitter()->editTabWidget();
    int len = tabs->editWidgetList()->length();
    for (int i = len -1; i >= 0; i-- ) {
        tabs->removeTab(i);
    }

    a->quit();
}

void ScoreMenu::onOpenActionTriggered()
{
    QString  home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open ABC Score"), home, tr("ABC score (*.abc)"));

    /* user cancelled */
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        AbcApplication* a = static_cast<AbcApplication*>(qApp);
        AbcMainWindow* w = a->mainWindow();
        EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();

        EditWidget* widget = new EditWidget(fileName, nullptr);

        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
        edit->setPlainText(file.readAll());
        file.close();

        edittabs->addTab(widget);
    }
}

void ScoreMenu::onSaveActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();

    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    QString fileName = (*edittabs->currentEditWidget()->fileName());
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not save an untitled ABC file!"));
        return;
    }

    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        AbcApplication* a = static_cast<AbcApplication*>(qApp);
        EditTabWidget *edittabs = a->mainWindow()->mainHSplitter()->editTabWidget();
        EditWidget* widget = static_cast<EditWidget*>(edittabs->currentWidget());
        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
        QString tosave = edit->toPlainText();
        file.write(tosave.toUtf8());
        file.close();
        w->statusBar()->showMessage(tr("Score saved."));
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Could not save ABC score!"));
    }
}

void ScoreMenu::onSaveAsActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();
    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    QString  home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save ABC score"), home, tr("ABC score (*.abc)"));
    if (fileName.isEmpty())
        return; /* cancelled */

    QFileInfo info(fileName);
    edittabs->setTabText(edittabs->currentIndex(), info.baseName());
    edittabs->currentEditWidget()->setFileName(fileName);
    return onSaveActionTriggered();
}

void ScoreMenu::onExportActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();
    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    QString  home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export MIDI file"), home, tr("MIDI file (*.mid)"));
    if (fileName.isEmpty())
        return; /* cancelled */

    EditWidget* ew = edittabs->editWidgetList()->at(cur);
    ew->editVBoxLayout()->exportMIDI(fileName);
}

void ScoreMenu::onCloseActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    if (QMessageBox::StandardButton::No == QMessageBox::question(a->mainWindow(), tr("Really close?"), tr("Do you really want to close this score?")))
        return;

    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();
    int cur = edittabs->currentIndex();

    if (cur >= 0)
        edittabs->removeTab(cur);
}

void ScoreMenu::onNewActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();

    QString empty;
    EditWidget* swidget = new EditWidget(empty, nullptr);
    swidget->editVBoxLayout()->abcPlainTextEdit()->setPlainText(NEW_TEMPLATE);

    edittabs->addTab(swidget);
}
