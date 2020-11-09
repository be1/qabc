#ifndef NEWACTION_H
#define NEWACTION_H

#include <QAction>

class NewAction: public QAction
{
	Q_OBJECT

public:
	NewAction(QWidget* parent = nullptr);
	~NewAction();

private:
};
#endif
