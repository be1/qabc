#ifndef SAVEASACTION_H
#define SAVEASACTION_H

#include <QAction>

class SaveasAction: public QAction
{
	Q_OBJECT

public:
	SaveasAction(QWidget* parent = nullptr);
	~SaveasAction();

private:
};
#endif
