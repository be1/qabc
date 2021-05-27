#ifndef SCOREMENU_H
#define SCOREMENU_H

#include <QMessageBox>
#include <QAction>
#include <QMenu>

class ScoreMenu: public QMenu
{
	Q_OBJECT

public:
	ScoreMenu(QWidget* parent = nullptr);
	~ScoreMenu();
	QMessageBox::StandardButton gracefulQuit();	

protected slots:
    void onQuitActionTriggered();
    void onOpenActionTriggered();
    void onSaveActionTriggered();
    void onSaveAsActionTriggered();
    void onCloseActionTriggered();
    void onNewActionTriggered();

private:
    QAction newaction;
    QAction openaction;
    QAction saveaction;
    QAction saveasaction;
    QAction closeaction;
    QAction quitaction;
};

#define NEW_TEMPLATE "X:1\nT:Melody...\nC:Piotr Ilitch Tchaïkovski\nM:4/4\nL:1/4\nK:Amin\n!mf!ABcd|e3c|e3A|cAFc|A4|]"
#endif
