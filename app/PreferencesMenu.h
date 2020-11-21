#ifndef PREFERENCESMENU_H
#define PREFERENCESMENU_H

#include "MIDIPrefAction.h"
#include "SfontPrefAction.h"
#include "AdriverPrefAction.h"
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
    void onPlayerActionTriggered();
    void onAdriverActionTriggered();
    void onSfontActionTriggered();
    void onResetActionTriggered();

private:
    MIDIPrefAction playeraction;
    AdriverPrefAction adriveraction;
    ResetAction resetaction;
    SfontPrefAction sfontaction;
};
#endif
