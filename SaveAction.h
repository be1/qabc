#ifndef SAVEACTION_H
#define SAVEACTION_H

#include <QAction>

class SaveAction: public QAction
{
	Q_OBJECT

public:
	SaveAction(QWidget* parent = nullptr);
	~SaveAction();

private:
};
#endif
