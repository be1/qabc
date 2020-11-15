#ifndef QUITACTION_H
#define QUITACTION_H

#include <QAction>

class QuitAction: public QAction
{
	Q_OBJECT

public:
	QuitAction(QWidget* parent = nullptr);
	~QuitAction();

private:
};
#endif
