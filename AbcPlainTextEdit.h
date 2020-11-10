#ifndef ABCPLAINTEXTEDIT_H
#define ABCPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

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
    QTextCharFormat keywordFormat;
    QTextCharFormat extraInstructionFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat noteFormat;
    QTextCharFormat chordFormat;
    QTextCharFormat barFormat;
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

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    AbcHighlighter *highlighter;
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
