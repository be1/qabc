#ifndef ABOUTQTACTION_H
#define ABOUTQTACTION_H

#include <QAction>

class AboutqtAction: public QAction
{
	Q_OBJECT

public:
	AboutqtAction(QWidget* parent = nullptr);
	~AboutqtAction();

private:
};
#endif
