#ifndef COMPILERACTION_H
#define COMPILERACTION_H

#include <QAction>

class PSPrefAction: public QAction
{
	Q_OBJECT

public:
    PSPrefAction(QWidget* parent = nullptr);
    ~PSPrefAction();

private:
};
#endif
