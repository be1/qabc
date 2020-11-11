#ifndef PREFERENCESMENU_H
#define PREFERENCESMENU_H

#include "MIDIPrefAction.h"
#include "PSPrefAction.h"
#include "SynthPrefAction.h"
#include "ResetAction.h"
#include <QMenu>

#define SETTINGS_DOMAIN "Herewe"
#define SETTINGS_APP "QAbc"
#define COMPILER_KEY "ABC/compiler"
#define ABC2MMIDI "/usr/bin/abc2midi"
#define PLAYER_KEY "ABC/player"
#define ABCM2PS "/usr/bin/abcm2ps -i -O="
#define SYNTH_KEY "ABC/synth"
#define FLUIDSYNTH "/usr/bin/fluidsynth --no-shell --audio-driver=alsa /usr/share/sounds/sf2/FluidR3_GM.sf2"
#define VIEWER_KEY "ABC/viewer"
#define PSVIEWER "/usr/bin/evince"

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
	PSPrefAction compileraction;
    MIDIPrefAction playeraction;
    SynthPrefAction synthaction;
    ResetAction resetaction;
    QAction vieweraction;
};
#endif
