#include "SynthPrefAction.h"

SynthPrefAction::SynthPrefAction(QWidget* parent)
	: QAction(parent)
{
    setText(tr("MIDI Synthesizer"));
}

SynthPrefAction::~SynthPrefAction()
{
}
