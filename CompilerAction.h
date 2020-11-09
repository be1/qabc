#ifndef COMPILERACTION_H
#define COMPILERACTION_H

#include <QAction>

class CompilerAction: public QAction
{
	Q_OBJECT

public:
	CompilerAction(QWidget* parent = nullptr);
	~CompilerAction();

private:
};
#endif
