#ifndef PREFERENCESMENU_H
#define PREFERENCESMENU_H

#include "MIDIPrefAction.h"
#include "PSPrefAction.h"
#include "SynthPrefAction.h"
#include "ResetAction.h"
#include "config.h"
#include <QMenu>

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

private:
    MIDIPrefAction playeraction;
    SynthPrefAction synthaction;
    ResetAction resetaction;
#if 0
    PSPrefAction compileraction;
    QAction vieweraction;
#endif
};
#endif
