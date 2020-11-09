#ifndef OPENACTION_H
#define OPENACTION_H

#include <QAction>

class OpenAction: public QAction
{
	Q_OBJECT

public:
	OpenAction(QWidget* parent = nullptr);
	~OpenAction();

private:
};
#endif
