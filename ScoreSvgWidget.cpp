#include "ScoreSvgWidget.h"
#include <QSvgRenderer>
#include <QPainter>

ScoreSvgWidget::ScoreSvgWidget(QWidget *parent)
    : QSvgWidget(parent)
{

}
#if 0
void ScoreSvgWidget::dumpImage(const QString &outfileName)
{
    QImage * img = new QImage(420, 594, QImage::Format_ARGB32);
    QPainter painter(img);
    painter.fillRect(img->rect(), QColor("white"));
    renderer()->render(&painter);
    img->save(outfileName);
}
#endif
