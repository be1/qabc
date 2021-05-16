# QAbc
ABC music notation minimal GUI, the SVG and fluid version (v2).

## Dependencies
QAbc uses third-party software to generate MIDI, AUDIO, and SVG files. Without them, QAbc would be very poor (only a little fancy text editor). So, please install them:
- abc2midi (program, not mandatory)
- drumstick-file (library and headers)
- fluidsynth (library and headers)

Note that this version of QAbc uses a *modified* version of the open source abcm2ps code, maintained in a specific submodule for QAbc.
Original `abcm2ps` code is Copyright © 2014-2016 Jean-Francois Moine. 

## Screenshot
http://brouits.free.fr/images/qabc_v2alpha.png

## Setting up
```
git clone https://github.com/be1/qabc.git
git checkout fluid
cd abcm2ps
git submodule init
git submodule update
git checkout lib
cd ..
```
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

## Thanks
QProgressIndicator is from Morgan Leborgne, thanks to him for this free piece of software. https://github.com/mojocorp/QProgressIndicator

Icon is derived from johnny\_automatic (from the Lulu Alphabet, 1867).
https://openclipart.org/detail/6264/harp-and-branch 

