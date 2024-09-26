// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ScoreMenu.h"
#include "AbcApplication.h"
#include "AbcPlainTextEdit.h"
#include "EditTabWidget.h"
#include "EditWidget.h"
#include "settings.h"
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

ScoreMenu::ScoreMenu(QWidget* parent)
	: QMenu(parent)
{
    setTitle(tr("Score"));

#if (QT_VERSION < QT_VERSION_CHECK(6, 3, 0))
    addAction(tr("New"), this, SLOT(onNewActionTriggered()), QKeySequence::New);
    addAction(tr("Open"), this, SLOT(onOpenActionTriggered()), QKeySequence::Open);
#else
    addAction(tr("New"), QKeySequence::New, this, SLOT(onNewActionTriggered()));
    addAction(tr("Open"), QKeySequence::Open, this, SLOT(onOpenActionTriggered()));
#endif

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], &QAction::triggered, this, &ScoreMenu::onOpenRecentActionTriggered);
    }

    QMenu* sub = addMenu(tr("Recently opened"));
    for (int i = 0; i < MaxRecentFiles; ++i)
        sub->addAction(recentFileActs[i]);

    updateRecentFileActions();

#if (QT_VERSION < QT_VERSION_CHECK(6, 3, 0))
    addAction(tr("Save"), this, SLOT(onSaveActionTriggered()), QKeySequence::Save);
    addAction(tr("Save as"), this, SLOT(onSaveAsActionTriggered()), QKeySequence::SaveAs);
    addAction(tr("Close"), this, SLOT(onCloseActionTriggered()), QKeySequence::Close);
    addAction(tr("Quit"), this, SLOT(onQuitActionTriggered()), QKeySequence::Quit);
#else
    addAction(tr("Save"), QKeySequence::Save, this, SLOT(onSaveActionTriggered()));
    addAction(tr("Save as"), QKeySequence::SaveAs, this, SLOT(onSaveAsActionTriggered()));
    addAction(tr("Close"), QKeySequence::Close, this, SLOT(onCloseActionTriggered()));
    addAction(tr("Quit"), QKeySequence::Quit, this, SLOT(onQuitActionTriggered()));
#endif
}

ScoreMenu::~ScoreMenu()
{
}

QMessageBox::StandardButton ScoreMenu::gracefulQuit()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditTabWidget* tabs = a->mainWindow()->mainHBoxLayout()->editTabWidget();

    int unsaved= 0;
    for (int i = 0; i < tabs->editWidgetList()->length(); i++) {
        if (!tabs->editWidgetList()->at(i)->editVBoxLayout()->abcPlainTextEdit()->isSaved())
            unsaved++;
    }

    if (unsaved && QMessageBox::StandardButton::No == QMessageBox::question(a->mainWindow(), tr("Really quit?"),
                                                                            QString::number(unsaved) +
                                                                            tr(" score(s) not saved.\nDo you want to quit anyway?")))
        return QMessageBox::StandardButton::No;

    return QMessageBox::StandardButton::Yes;

}

void ScoreMenu::onQuitActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditTabWidget* tabs = a->mainWindow()->mainHBoxLayout()->editTabWidget();
    if (QMessageBox::StandardButton::Yes == gracefulQuit()) {
            tabs->removeTabs();
            a->quit();
    }
}

void ScoreMenu::onOpenActionTriggered()
{
    QString  home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open ABC Score"), home, tr("ABC score (*.abc)"));

    /* user cancelled */
    if (fileName.isEmpty())
        return;

    loadFile(fileName);
}

QString ScoreMenu::strippedName(const QString& fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void ScoreMenu::updateRecentFileActions()
{
    Settings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = QString("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
}

void ScoreMenu::setRecentFile(const QString& fileName, bool ok)
{
    Settings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);

    if (ok)
        files.prepend(fileName);

    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);
    updateRecentFileActions();
}

bool ScoreMenu::loadFile(const QString& fileName)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();

    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        EditTabWidget *edittabs = w->mainHBoxLayout()->editTabWidget();

        EditWidget* widget = new EditWidget(fileName, edittabs);

        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
        edit->setPlainText(file.readAll());
        file.close();
        edit->setSaved();
        edittabs->addTab(widget);

        setRecentFile(fileName, true);

        return true;
    }

    setRecentFile(fileName, false);
    return false;
}

void ScoreMenu::onOpenRecentActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
        if (!loadFile(action->data().toString())) {
            QMessageBox::warning(w, tr("Warning"), tr("Could not open score!"));
        }
    }
}

void ScoreMenu::onSaveActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHBoxLayout()->editTabWidget();

    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    QString fileName = (*edittabs->currentEditWidget()->fileName());
    if (fileName.isEmpty()) {
        QMessageBox::warning(w, tr("Warning"), tr("Could not save an untitled ABC score!"));
        return;
    }

    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        AbcApplication* a = static_cast<AbcApplication*>(qApp);
        EditTabWidget *edittabs = a->mainWindow()->mainHBoxLayout()->editTabWidget();
        EditWidget* widget = static_cast<EditWidget*>(edittabs->currentWidget());
        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
        QString tosave = edit->toPlainText();
        file.write(tosave.toUtf8());
        file.close();
        edit->setSaved();
        w->statusBar()->showMessage(tr("Score saved."));
        setRecentFile(fileName, true);
    } else {
        QMessageBox::warning(w, tr("Warning"), tr("Could not save ABC score!"));
    }
}

void ScoreMenu::onSaveAsActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHBoxLayout()->editTabWidget();
    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    QString  home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save ABC score"), home, tr("ABC score (*.abc)"));
    if (fileName.isEmpty())
        return; /* cancelled */

    if (!fileName.endsWith(".abc"))
        fileName.append(".abc");

    QFileInfo info(fileName);
    edittabs->setTabText(edittabs->currentIndex(), info.baseName());
    edittabs->currentEditWidget()->setFileName(fileName);

    return onSaveActionTriggered();
}

void ScoreMenu::onCloseActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHBoxLayout()->editTabWidget();
    int cur = edittabs->currentIndex();
    if (cur >= 0)
        edittabs->askRemoveTab(cur);
}

void ScoreMenu::onNewActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHBoxLayout()->editTabWidget();

    QString empty;
    EditWidget* swidget = new EditWidget(empty, nullptr);
    swidget->editVBoxLayout()->abcPlainTextEdit()->setPlainText(NEW_TEMPLATE);

    edittabs->addTab(swidget);
}
