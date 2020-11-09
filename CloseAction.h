#ifndef CLOSEACTION_H
#define CLOSEACTION_H

#include <QAction>

class CloseAction: public QAction
{
	Q_OBJECT

public:
	CloseAction(QWidget* parent = nullptr);
	~CloseAction();

private:
};
#endif
