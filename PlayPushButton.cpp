#include "PlayPushButton.h"

PlayPushButton::PlayPushButton(QWidget* parent)
	: QPushButton(parent)
{
    play = true;
    setText(tr("Play"));
}

PlayPushButton::~PlayPushButton()
{
}

void PlayPushButton::flip()
{
    if (play)
        setText(tr("Stop"));
    else
        setText(tr("Play"));

    play = !play;
}

bool PlayPushButton::isPlay()
{
    return play;
}
