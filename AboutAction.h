#ifndef ABOUTACTION_H
#define ABOUTACTION_H

#include <QAction>

class AboutAction: public QAction
{
	Q_OBJECT

public:
	AboutAction(QWidget* parent = nullptr);
	~AboutAction();

private:
};
#endif
