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
