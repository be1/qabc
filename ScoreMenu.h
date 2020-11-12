#ifndef SCOREMENU_H
#define SCOREMENU_H

#include "QuitAction.h"
#include "CloseAction.h"
#include "SaveasAction.h"
#include "SaveAction.h"
#include "OpenAction.h"
#include "NewAction.h"
#include <QMenu>

class ScoreMenu: public QMenu
{
	Q_OBJECT

public:
	ScoreMenu(QWidget* parent = nullptr);
	~ScoreMenu();

protected slots:
    void onQuitActionTriggered();
    void onOpenActionTriggered();
    void onSaveActionTriggered();
    void onSaveAsActionTriggered();
    void onCloseActionTriggered();
    void onNewActionTriggered();

private:
	NewAction newaction;
	OpenAction openaction;
	SaveAction saveaction;
	SaveasAction saveasaction;
	CloseAction closeaction;
    QuitAction quitaction;
};

#define NEW_TEMPLATE "X:1\nT:Melody...\nC:Piotr Ilitch Tchaïkovski\nM:4/4\nL:1/4\nK:Amin\n!mf!ABcd|e3c|e3A|cAFc|A4|]"
#endif
