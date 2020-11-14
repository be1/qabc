#include "ViewVSplitter.h"
#include <QFileInfo>
#include <QScrollArea>
#include <QDir>
#include <QDebug>

ViewVSplitter::ViewVSplitter(QWidget* parent)
    : QSplitter(parent)
    , prev(QIcon::fromTheme("previous"), tr("Previous"), this)
    , next(QIcon::fromTheme("next"), tr("Next"), this)
{
    setOrientation(Qt::Vertical);

    /* configure the svg score */
    QPalette p = svgwidget.palette();
    p.setColor(svgwidget.backgroundRole(), Qt::white);
    svgwidget.setPalette(p);
    svgwidget.setAutoFillBackground(true);
    svgwidget.setMinimumSize(630, 891);
    QSizePolicy pol;
    pol.setHeightForWidth(true);
    pol.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
    svgwidget.setSizePolicy(pol);

    /* configure the scroll area */
    QScrollArea *area = new QScrollArea();
    area->setWidgetResizable(true);
    area->setWidget(&svgwidget);

    /* configure the pages buttons */
    QWidget *pagesWidget = new QWidget(this);
    pagesWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    connect(&prev, &QPushButton::clicked, this, &ViewVSplitter::prevPageClicked);
    pageslayout.addWidget(&prev, 0, Qt::AlignLeft);
    connect(&next, &QPushButton::clicked, this, &ViewVSplitter::nextPageClicked);
    pageslayout.addWidget(&next, 0, Qt::AlignRight);
    pagesWidget->setLayout(&pageslayout);
    pagesWidget->setFixedHeight(pageslayout.sizeHint().height());

    /* add all in good order to the splitter */
    addWidget(area);
    addWidget(pagesWidget);
    addWidget(&logview);
    QList<int> sizes;
    sizes.append(594);
    sizes.append(100);
    setSizes(sizes);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setCollapsible(2, false);
}

ViewVSplitter::~ViewVSplitter()
{
}

LogView *ViewVSplitter::logView()
{
    return &logview;
}

void ViewVSplitter::initBasename(const QString &b)
{
    qDebug() << "check" << b;
    basename = b;
    currentpage = 0;
    for (int i = 1; i <= 999; i++) {
        QString nnn = QString::asprintf("%03d", i);
        QString temp(b);
        temp += nnn;
        temp += ".svg";
        if (!QFile::exists(temp)) {
            lastpage = i - 1;
            break;
        }
    }
}

void ViewVSplitter::requestPage(int i) {
    int page = i + currentpage;
    if (page > 0 && page <= lastpage) {
        currentpage = page;
        QString nnn = QString::asprintf("%03d", page);
        QString temp(basename);
        temp += nnn;
        temp += ".svg";
        svgWidget()->load(temp);

        if (page > 1)
            prev.setEnabled(true);
        else
            prev.setEnabled(false);

        if (page < lastpage)
            next.setEnabled(true);
        else
            next.setEnabled(false);
    }
}

ScoreSvgWidget *ViewVSplitter::svgWidget()
{
    return &svgwidget;
}

void ViewVSplitter::prevPageClicked()
{
    requestPage(-1);
}


void ViewVSplitter::nextPageClicked()
{
    requestPage(1);
}
