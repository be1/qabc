#include "PlayPushButton.h"

PlayPushButton::PlayPushButton(QWidget* parent)
	: QPushButton(parent)
{
    play = true;
    setText(tr(TEXT_PLAY));
}

PlayPushButton::~PlayPushButton()
{
}

void PlayPushButton::flip()
{
    if (play)
        setText(tr(TEXT_STOP));
    else
        setText(tr(TEXT_PLAY));

    play = !play;
}

bool PlayPushButton::isPlay()
{
    return play;
}
