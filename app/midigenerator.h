#ifndef MIDIGENERATOR_H
#define MIDIGENERATOR_H

#include "generator.h"
#include <QObject>

class MidiGenerator : public Generator
{
    Q_OBJECT
public:
    MidiGenerator(QObject* parent = nullptr);
    void generate(const QString& input, int xopt, const QString& output, int cont) override;
protected:
    void spawnMidiCompiler(const QString &prog, const QStringList& args, const QDir &wrk, int cont);

};

#endif // MIDIGENERATOR_H
