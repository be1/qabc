#ifndef VIEWERACTION_H
#define VIEWERACTION_H

#include <QAction>

class SynthAction: public QAction
{
	Q_OBJECT

public:
	SynthAction(QWidget* parent = nullptr);
	~SynthAction();

private:
};
#endif
