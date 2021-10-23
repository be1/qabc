/*
 *   This file is part of QAbc.
 *
 *   QAbc is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   QAbc is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with QAbc.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "settings.h"
#include "config.h"

Settings::Settings(QObject* parent)
    : QSettings(SETTINGS_DOMAIN, SETTINGS_APP, parent)
{

}

void Settings::check()
{

    QVariant player = value(PLAYER_KEY);
    if (!player.isValid())
        setValue(PLAYER_KEY, ABC2MIDI);

    QVariant synth = value(SYNTH_KEY);
    if (!synth.isValid())
        setValue(SYNTH_KEY, FLUIDSYNTH);

    QVariant compiler = value(COMPILER_KEY);
    if (!compiler.isValid())
        setValue(COMPILER_KEY, ABCM2PS);

    QVariant viewer = value(VIEWER_KEY);
    if (viewer.isValid())
        setValue(VIEWER_KEY, PSVIEWER);

    QVariant highlight = value(EDITOR_HIGHLIGHT);
    if (!highlight.isValid())
        setValue(EDITOR_HIGHLIGHT, true);

    QVariant color = value(EDITOR_BAR_COLOR);
    if (!color.isValid())
        setValue(EDITOR_BAR_COLOR, "red");

    color = value(EDITOR_COMMENT_COLOR);
    if (!color.isValid())
        setValue(EDITOR_COMMENT_COLOR, "gray");

    color = value(EDITOR_DECORATION_COLOR);
    if (!color.isValid())
        setValue(EDITOR_DECORATION_COLOR, "orange");

    color = value(EDITOR_EXTRAINSTR_COLOR);
    if (!color.isValid())
        setValue(EDITOR_EXTRAINSTR_COLOR, "blue");

    color = value(EDITOR_GCHORD_COLOR);
    if (!color.isValid())
        setValue(EDITOR_GCHORD_COLOR, "green");

    color = value(EDITOR_HEADER_COLOR);
    if (!color.isValid())
        setValue(EDITOR_HEADER_COLOR, "darkcyan");

    color = value(EDITOR_LYRIC_COLOR);
    if (!color.isValid())
        setValue(EDITOR_LYRIC_COLOR, "magenta");

    sync();
}

void Settings::reset()
{
    setValue(PLAYER_KEY, ABC2MIDI);

    setValue(SYNTH_KEY, FLUIDSYNTH);

    setValue(COMPILER_KEY, ABCM2PS);

    setValue(VIEWER_KEY, PSVIEWER);

    setValue(EDITOR_HIGHLIGHT, true);

    setValue(EDITOR_BAR_COLOR, "red");

    setValue(EDITOR_COMMENT_COLOR, "gray");

    setValue(EDITOR_DECORATION_COLOR, "orange");

    setValue(EDITOR_EXTRAINSTR_COLOR, "blue");

    setValue(EDITOR_GCHORD_COLOR, "green");

    setValue(EDITOR_HEADER_COLOR, "darkcyan");

    setValue(EDITOR_LYRIC_COLOR, "magenta");

    sync();

}
