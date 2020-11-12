#include "AbcPlainTextEdit.h"
#include <QDebug>
#include <QPainter>
#include <QTextBlock>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QStringListModel>
#include <QGuiApplication>

AbcPlainTextEdit::AbcPlainTextEdit(QWidget* parent)
	: QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    highlighter = new AbcHighlighter(this->document());
    QCompleter *com = new QCompleter(this);
    com->setModel(modelFromFile(":dict.txt"));
    com->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    com->setCaseSensitivity(Qt::CaseInsensitive);
    com->setWrapAround(false);
    setCompleter(com);

    connect(this, &AbcPlainTextEdit::blockCountChanged, this, &AbcPlainTextEdit::updateLineNumberAreaWidth);
    connect(this, &AbcPlainTextEdit::updateRequest, this, &AbcPlainTextEdit::updateLineNumberArea);
    connect(this, &AbcPlainTextEdit::cursorPositionChanged, this, &AbcPlainTextEdit::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

AbcPlainTextEdit::~AbcPlainTextEdit()
{
}

void AbcPlainTextEdit::setCompleter(QCompleter *completer)
{
    if (c)
        c->disconnect(this);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseSensitive);
    QObject::connect(c, QOverload<const QString &>::of(&QCompleter::activated),
                     this, &AbcPlainTextEdit::insertCompletion);
}

QCompleter *AbcPlainTextEdit::completer() const
{
    return c;
}

void AbcPlainTextEdit::insertCompletion(const QString &completion)
{
    if (c->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - c->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString AbcPlainTextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}


void AbcPlainTextEdit::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void AbcPlainTextEdit::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    /* Here, the completer popup is not shown yet */

    //const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    const bool isShortcut = e->key() == Qt::Key_Tab && e->modifiers().testFlag(Qt::NoModifier);
    if (!c || !isShortcut)
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("!%~@#$^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    /* no shortcut pressed, or a modifier-only key is pressed,
     * or the word typed is too short, or it is a complete word:
     * then unshow popup if needed and return */
    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                      || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    /* here we have a possible case of showing completion popup */
    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}

int AbcPlainTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void AbcPlainTextEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void AbcPlainTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void AbcPlainTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void AbcPlainTextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void AbcPlainTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

AbcHighlighter::AbcHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    AbcHighlightingRule rule;

    barFormat.setFontWeight(QFont::Bold);
    barFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression(QStringLiteral("(::|[:\\|\\[]?\\|[:\\|\\]]?)"));
    rule.format = barFormat;
    highlightingRules.append(rule);

    noteFormat.setFontWeight(QFont::Bold);
    noteFormat.setForeground(Qt::darkGray);
    rule.pattern = QRegularExpression(QStringLiteral("[_=^]*[A-HZa-hz][,']*/*[1-9]*"));
    rule.format = noteFormat;
    highlightingRules.append(rule);

    chordFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\"[A-H][^\"]*\""));
    rule.format = chordFormat;
    highlightingRules.append(rule);

    extraInstructionFormat.setFontItalic(true);
    extraInstructionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression(QStringLiteral("^%%[^\n]*"));
    rule.format = extraInstructionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression(QStringLiteral("^%[^%\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    keywordFormat.setForeground(Qt::darkMagenta);
    keywordFormat.setFontWeight(QFont::Thin);
    const QString keywordPatterns[] = {
        QStringLiteral("^A:[^\n]+"), QStringLiteral("^B:[^\n]+"), QStringLiteral("^C:[^\n]+"),
        QStringLiteral("^D:[^\n]+"), QStringLiteral("^E:[^\n]+"), QStringLiteral("^F:[^\n]+"),
        QStringLiteral("^G:[^\n]+"), QStringLiteral("^H:[^\n]+"), QStringLiteral("^I:[^\n]+"),
        QStringLiteral("^K:[^\n]+"), QStringLiteral("^L:[^\n]+"), QStringLiteral("^M:[^\n]+"),
        QStringLiteral("^N:[^\n]+"), QStringLiteral("^O:[^\n]+"), QStringLiteral("^P:[^\n]+"),
        QStringLiteral("^Q:[^\n]+"), QStringLiteral("^R:[^\n]+"), QStringLiteral("^S:[^\n]+"),
        QStringLiteral("^T:[^\n]+"), QStringLiteral("^V:[^\n]+"), QStringLiteral("^W:|^\n]+"),
        QStringLiteral("^w:[^\n]+"), QStringLiteral("^X:[^\n]+"), QStringLiteral("^Z:[^\n]+")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

#if 0
    multiLineHeaderFormat.setForeground(Qt::darkMagenta);

    headerStartExpression = QRegularExpression(QStringLiteral("^X:"));
    headerEndExpression = QRegularExpression(QStringLiteral("^K:[^\n]+"));
#endif
}

void AbcHighlighter::highlightBlock(const QString &text)
{
    for (const AbcHighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
#if 0
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(headerStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = headerEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int headerLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            headerLength = text.length() - startIndex;
        } else {
            headerLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, headerLength, multiLineHeaderFormat);
        startIndex = text.indexOf(headerStartExpression, startIndex + headerLength);
    }
#endif
}

QAbstractItemModel *AbcPlainTextEdit::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(c);

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << QString::fromUtf8(line.trimmed());
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, c);
}
