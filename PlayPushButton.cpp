// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PlayPushButton.h"

PlayPushButton::PlayPushButton(QWidget* parent)
	: QPushButton(parent)
{
    play = true;
    setText(tr("&Play"));
    setIcon(QIcon::fromTheme("media-playback-start"));
}

PlayPushButton::~PlayPushButton()
{
}

void PlayPushButton::flip()
{
    if (play) {
        setText(tr("Sto&p"));
        setIcon(QIcon::fromTheme("media-playback-stop"));
    } else {
        setText(tr("&Play"));
        setIcon(QIcon::fromTheme("media-playback-start"));
    }

    play = !play;
}

bool PlayPushButton::isPlay()
{
    return play;
}
