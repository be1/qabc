#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include "AbcApplication.h"
#include "midigenerator.h"
#include "generator.h"

MidiGenerator::MidiGenerator(QObject* parent)
    : Generator(parent)
{

}

void MidiGenerator::generate(const QString &input, int xopt, QString output, int cont)
{
    QSettings settings(SETTINGS_DOMAIN, SETTINGS_APP);
    QVariant player = settings.value(PLAYER_KEY);

    QString program = player.toString();
    QStringList argv = program.split(" ");
    program = argv.at(0);

    if (!QFileInfo::exists(program)) {
        emit generated(1, cont);
        return;
    }

    if (output.isEmpty())
        output = input;
        output.replace(QRegularExpression("\\.abc$"), QString::number(xopt) + ".mid");

    argv.removeAt(0);
    argv << input;
    argv << QString::number(xopt);
    argv << "-v";
    argv << "-o" << output;
    QFileInfo info(input);
    QDir dir = info.absoluteDir();

    spawnMidiCompiler(program, argv, dir, cont);
}

void MidiGenerator::spawnMidiCompiler(const QString& prog, const QStringList &args, const QDir &wrk, int cont)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow *w = a->mainWindow();
    w->mainHSplitter()->viewWidget()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessPlayer, wrk, cont);
}
