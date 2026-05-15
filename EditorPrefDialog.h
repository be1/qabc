// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EDITORPREFDIALOG_H
#define EDITORPREFDIALOG_H

#include <QSpinBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QCheckBox>
#include <QFontComboBox>

class EditorPrefDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditorPrefDialog(QWidget *parent = nullptr);
    ~EditorPrefDialog();

    QColor getColor(QString key);
    bool getHighlight();
    int getFontRange();
    QFont getBaseFont();

signals:

private slots:
    void onColorButtonClicked();

private:
    QVBoxLayout* mainLayout;

    QLabel* fontLabel;
    QFontComboBox* fontBaseCombo;

    QLabel* fontRangeLabel;
    QSpinBox* fontRangeSpinBox;

    QLabel* highlightLabel;
    QCheckBox* highlightCheck;

    QStringList colorLabels;
    QStringList colorKeys;
    QList<QPushButton*> colorButtons;

    QDialogButtonBox* buttons;
};

#endif // EDITORPREFDIALOG_H
