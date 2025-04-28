# QAbc
ABC music notation minimal GUI.

## Dependencies
QAbc uses third-party softwares to generate MIDI, AUDIO, and PostScript files. Without them, QAbc would be very poor (only a little fancy text editor). So, please install them:

- abc2midi (CLI)
- fluidsynth (CLI)
- abcm2ps (CLI)
- Qt 5.x or 6.x (LIB)

## Screenshot
![screenshot](http://brouits.free.fr/qabc/qabc.png "The main window")

## Building
Just a matter of:
```
$ cd qabc
$ qmake -config release
$ make
# make install (as root)
```

## Starting
Launch `qabc`, type some ABC music in the editor, press `Play` button: you're done!

## Bugs
Lots of!

## Colophon
This project is (was), as of version 0.1, a one-night project because I wanted to be more comfortable with ABC notation and I could not find a GUI software that could run on my laptop.

## Icon
Icon is derived from [Cris Gradina's harp.svg](https://openclipart.org/detail/182170/harp)

