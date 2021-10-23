/*
 *   This file is part of QAbc.
 *
 *   QAbc is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   QAbc is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with QAbc.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef EDITORPREFDIALOG_H
#define EDITORPREFDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QCheckBox>

class EditorPrefDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditorPrefDialog(QWidget *parent = nullptr);
    ~EditorPrefDialog();

    QColor getColor(QString key);
    bool getHighlight();

signals:

private slots:
    void onColorButtonClicked();

private:
    QVBoxLayout* mainLayout;

    QLabel* highlightLabel;
    QCheckBox* highlightCheck;

    QStringList colorLabels;
    QStringList colorKeys;
    QList<QPushButton*> colorButtons;

    QDialogButtonBox* buttons;
};

#endif // EDITORPREFDIALOG_H
