#include "CompilerAction.h"

CompilerAction::CompilerAction(QWidget* parent)
	: QAction(parent)
{
	setText(tr("Compiler"));
}

CompilerAction::~CompilerAction()
{
}
