#ifndef EDITORACTION_H
#define EDITORACTION_H

#include <QAction>

class ResetAction: public QAction
{
	Q_OBJECT

public:
	ResetAction(QWidget* parent = nullptr);
	~ResetAction();

private:
};
#endif
