#ifndef PLAYERACTION_H
#define PLAYERACTION_H

#include <QAction>

class MIDIPrefAction: public QAction
{
	Q_OBJECT

public:
    MIDIPrefAction(QWidget* parent = nullptr);
    ~MIDIPrefAction();

private:
};
#endif
