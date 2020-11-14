#ifndef SCORESVGWIDGET_H
#define SCORESVGWIDGET_H

#include <QSvgWidget>


class ScoreSvgWidget : public QSvgWidget
{
    Q_OBJECT
public:
    ScoreSvgWidget(QWidget *parent = nullptr);
    int heightForWidth(int w) const {
        return w * 297 / 210;
    };

    //void dumpImage(const QString& outfileName);
};

#endif // SCORESVGWIDGET_H
