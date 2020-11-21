#ifndef COMPILERACTION_H
#define COMPILERACTION_H

#include <QAction>

class SfontPrefAction: public QAction
{
	Q_OBJECT

public:
    SfontPrefAction(QWidget* parent = nullptr);
    ~SfontPrefAction();

private:
};
#endif
