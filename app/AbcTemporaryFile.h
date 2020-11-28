#ifndef ABCTEMPORARYFILE_H
#define ABCTEMPORARYFILE_H

#include <QTemporaryFile>

class AbcTemporaryFile : public QTemporaryFile
{
    Q_OBJECT
public:
    AbcTemporaryFile();

    bool open() { return open(QIODevice::WriteOnly); };

protected:
    bool open(OpenMode mode) { return QTemporaryFile::open(mode);};
};

#endif // ABCTEMPORARYFILE_H
