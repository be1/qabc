#ifndef VIEWERACTION_H
#define VIEWERACTION_H

#include <QAction>

class AdriverPrefAction: public QAction
{
	Q_OBJECT

public:
    AdriverPrefAction(QWidget* parent = nullptr);
    ~AdriverPrefAction();

private:
};
#endif
