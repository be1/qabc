#ifndef VIEWERACTION_H
#define VIEWERACTION_H

#include <QAction>

class SynthPrefAction: public QAction
{
	Q_OBJECT

public:
    SynthPrefAction(QWidget* parent = nullptr);
    ~SynthPrefAction();

private:
};
#endif
