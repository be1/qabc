#include "SynthAction.h"

SynthAction::SynthAction(QWidget* parent)
	: QAction(parent)
{
    setText(tr("Synth"));
}

SynthAction::~SynthAction()
{
}
