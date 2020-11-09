#include "ScoreMenu.h"
#include "AbcApplication.h"
#include "AbcPlainTextEdit.h"
#include "EditTabWidget.h"
#include "EditWidget.h"
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>

ScoreMenu::ScoreMenu(QWidget* parent)
	: QMenu(parent)
{
	setTitle(tr("Score"));
	addAction(&newaction);
	addAction(&openaction);
	addAction(&saveaction);
	addAction(&saveasaction);
	addAction(&closeaction);
	addAction(&quitaction);

	connect(&quitaction, SIGNAL(triggered()), this, SLOT(onQuitActionTriggered()));
    connect(&openaction, SIGNAL(triggered()), this, SLOT(onOpenActionTriggered()));
    connect(&saveaction, SIGNAL(triggered()), this, SLOT(onSaveActionTriggered()));
    connect(&saveasaction, SIGNAL(triggered()), this, SLOT(onSaveAsActionTriggered()));
    connect(&closeaction, SIGNAL(triggered()), this, SLOT(onCloseActionTriggered()));
    connect(&newaction, SIGNAL(triggered()), this, SLOT(onNewActionTriggered()));
}

ScoreMenu::~ScoreMenu()
{
}

void ScoreMenu::onQuitActionTriggered()
{
    qApp->quit();
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
        CentralWidget* w = static_cast<CentralWidget*>(a->mainWindow()->centralWidget());
        EditTabWidget *edittabs = w->mainHBoxLayout()->editTabWidget();

        EditWidget* widget = new EditWidget(fileName, edittabs);
        edittabs->addTab(widget);

        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
        edit->setPlainText(file.readAll());
        file.close();
    }
}

void ScoreMenu::onSaveActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditTabWidget *edittabs = static_cast<CentralWidget*>(a->mainWindow()->centralWidget())->mainHBoxLayout()->editTabWidget();

    QString fileName = (*edittabs->currentEditWidget()->fileName());
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not save an untitled ABC score!"));
        return;
    }

    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        AbcApplication* a = static_cast<AbcApplication*>(qApp);
        EditTabWidget *edittabs = static_cast<CentralWidget*>(a->mainWindow()->centralWidget())->mainHBoxLayout()->editTabWidget();
        EditWidget* widget = static_cast<EditWidget*>(edittabs->currentWidget());
        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
        QString tosave = edit->toPlainText();
        file.write(tosave.toUtf8());
        file.close();
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Could not save ABC score!"));
    }
}

void ScoreMenu::onSaveAsActionTriggered()
{
    QString  home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save ABC score"), home, tr("ABC score (*.abc)"));
    if (fileName.isEmpty())
        return; /* cancelled */

    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditTabWidget *edittabs = static_cast<CentralWidget*>(a->mainWindow()->centralWidget())->mainHBoxLayout()->editTabWidget();
    QFileInfo info(fileName);
    edittabs->setTabText(edittabs->currentIndex(), info.baseName());
    edittabs->currentEditWidget()->setFileName(fileName);
    return onSaveActionTriggered();
}

void ScoreMenu::onCloseActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditTabWidget *edittabs = static_cast<CentralWidget*>(a->mainWindow()->centralWidget())->mainHBoxLayout()->editTabWidget();
    edittabs->removeTab(edittabs->currentIndex());
}

void ScoreMenu::onNewActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditTabWidget *edittabs = static_cast<CentralWidget*>(a->mainWindow()->centralWidget())->mainHBoxLayout()->editTabWidget();

    QString empty;
    EditWidget* swidget = new EditWidget(empty, edittabs);

    edittabs->addTab(swidget);
}
