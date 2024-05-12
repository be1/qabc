// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ABCPLAINTEXTEDIT_H
#define ABCPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QCompleter>

class AbcHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    AbcHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct AbcHighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<AbcHighlightingRule> highlightingRules;

#if 0
    QRegularExpression headerStartExpression;
    QRegularExpression headerEndExpression;
#endif
    QTextCharFormat headerFormat;
    QTextCharFormat extraInstructionFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat noteFormat;
    QTextCharFormat decorFormat;
    QTextCharFormat gchordFormat;
    QTextCharFormat barFormat;
    QTextCharFormat lyricFormat;
#if 0
    QTextCharFormat multiLineHeaderFormat;
#endif
};

class AbcPlainTextEdit: public QPlainTextEdit
{
	Q_OBJECT

public:
	AbcPlainTextEdit(QWidget* parent = nullptr);
    ~AbcPlainTextEdit();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

    bool isSaved();
    void setSaved();
    void findX(int x);


protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void flagModified(bool enable);
    virtual void contextMenuEvent(QContextMenuEvent *e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *e) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void checkDictionnary();
    void updateLineNumberArea(const QRect &rect, int dy);
    void insertCompletion(const QString &completion);
    void onFindActivated();
    void onFindForwardActivated();
    void onFindBackwardActivated();

private:
    QAbstractItemModel *modelFromFile(const QString &fileName);
    QAbstractItemModel *dictModel; /* normal dictionnary */
    QAbstractItemModel *psModel;   /* Rendering dictionnary */
    QAbstractItemModel *gmModel;   /* General MIDI dictionary */
    static const QString delimiter;

    QWidget *lineNumberArea;
    AbcHighlighter *highlighter;
    QString textUnderCursor() const;
    QString lineUnderCursor() const;
    QString charBeforeCursor(QTextCursor tc) const;
    QString wordBeforeCursor(QTextCursor tc) const;

    QString m_find;
    QAction* findaction;
    QAction* findnextaction;
    QAction* findprevaction;

    QCompleter *c = nullptr;
    bool saved;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(AbcPlainTextEdit *editor) : QWidget(editor), abcplaintextedit(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(abcplaintextedit->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        abcplaintextedit->lineNumberAreaPaintEvent(event);
    }

private:
    AbcPlainTextEdit *abcplaintextedit;
};

#endif
