#ifndef PLAYERACTION_H
#define PLAYERACTION_H

#include <QAction>

class PlayerAction: public QAction
{
	Q_OBJECT

public:
	PlayerAction(QWidget* parent = nullptr);
	~PlayerAction();

private:
};
#endif
