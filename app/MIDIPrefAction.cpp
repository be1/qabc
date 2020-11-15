#include "MIDIPrefAction.h"

MIDIPrefAction::MIDIPrefAction(QWidget* parent)
	: QAction(parent)
{
    setText(tr("MIDI Generator"));
}

MIDIPrefAction::~MIDIPrefAction()
{
}
