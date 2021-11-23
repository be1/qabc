// This source code is part of QAbc, a minimal ABC music notation editor.
// QAbc is Copyright © 2021 Benoît Rouits <brouits@free.fr>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ABCTEMPORARYFILE_H
#define ABCTEMPORARYFILE_H

#include <QTemporaryFile>

class AbcTemporaryFile : public QTemporaryFile
{
    Q_OBJECT
public:
    AbcTemporaryFile();

    bool open() { resize(0); return open(QIODevice::WriteOnly|QIODevice::Truncate); };

protected:
    bool open(OpenMode mode) { return QTemporaryFile::open(mode);};
};

#endif // ABCTEMPORARYFILE_H
