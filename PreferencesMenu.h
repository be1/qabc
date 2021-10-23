#ifndef PREFERENCESMENU_H
#define PREFERENCESMENU_H

#include <QAction>
#include <QMenu>
#include <config.h>

class PreferencesMenu: public QMenu
{
	Q_OBJECT

public:
	PreferencesMenu(QWidget* parent = nullptr);
	~PreferencesMenu();

protected slots:
    void onCompilerActionTriggered();
    void onPlayerActionTriggered();
    void onSynthActionTriggered();
    void onResetActionTriggered();
    void onViewerActionTriggered();
    void onEditorActionTriggered();

private:
    QAction compileraction;
    QAction playeraction;
    QAction synthaction;
    QAction resetaction;
    QAction vieweraction;
    QAction editoraction;
};
#endif
