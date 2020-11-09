#include "EditVBoxLayout.h"
#include "AbcApplication.h"
#include "PreferencesMenu.h"
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QSpinBox>
#include <QDir>

EditVBoxLayout::EditVBoxLayout(const QString& fileName, QWidget* parent)
    : QVBoxLayout(parent),
      fileName(fileName)
{
    xspinbox.setMinimum(1);
    xlabel.setText(tr("X:"));
    xlabel.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    xlabel.setBuddy(&xspinbox);
    hboxlayout.addWidget(&xlabel);
    hboxlayout.addWidget(&xspinbox);
    hboxlayout.addWidget(&playpushbutton);
    hboxlayout.addWidget(&runpushbutton);

    addWidget(&abcplaintextedit);
    addLayout(&hboxlayout);

    connect(&xspinbox, SIGNAL(valueChanged(int)), this, SLOT(onXChanged(int)));
    connect(&playpushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onPlayClicked);
    connect(&runpushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onRunClicked);

    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    connect(a, &AbcApplication::playerFinished, this, &EditVBoxLayout::onPlayFinished);
    connect(a, &AbcApplication::synthFinished, this, &EditVBoxLayout::onSynthFinished);
    connect(a, &AbcApplication::compilerFinished, this, &EditVBoxLayout::onCompileFinished);
}


EditVBoxLayout::~EditVBoxLayout()
{
}

AbcPlainTextEdit *EditVBoxLayout::abcPlainTextEdit()
{
    return &abcplaintextedit;
}

PlayPushButton *EditVBoxLayout::playPushButton()
{
    return &playpushbutton;
}

RunPushButton *EditVBoxLayout::runPushButton()
{
   return &runpushbutton;
}

void EditVBoxLayout::onXChanged(int value)
{
   qDebug() << value;
}

void EditVBoxLayout::onPlayClicked()
{
    playpushbutton.setEnabled(false);
    QSettings settings("Herewe", "QAbc");
    QVariant player = settings.value(PLAYER_KEY);
    QString program = player.toString();
    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    argv << fileName;
    QFileInfo info(fileName);
    QDir dir = info.absoluteDir();

    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    a->spawnPlayer(program, argv, dir);
}

void EditVBoxLayout::onPlayFinished(int exitCode)
{
    qDebug() << "play" << exitCode;
    if (exitCode)
        return;

    QSettings settings("Herewe", "QAbc");
    QVariant synth = settings.value(SYNTH_KEY);
    QString program = synth.toString();
    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    QString temp(fileName);
    argv << (temp.replace(".abc", QString::number(xspinbox.value())  + ".mid"));

    QFileInfo info(temp);
    QDir dir = info.absoluteDir();

    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    a->spawnSynth(program, argv, dir);

}

void EditVBoxLayout::onSynthFinished(int exitCode)
{
    qDebug() << "synth" << exitCode;
    playpushbutton.setEnabled(true);
}

void EditVBoxLayout::onRunClicked()
{
    runpushbutton.setEnabled(false);
    QSettings settings("Herewe", "QAbc");
    QVariant compiler = settings.value(COMPILER_KEY);
    QString program = compiler.toString();
    QStringList argv = program.split(" ");
    program = argv.at(0);
    argv.removeAt(0);
    argv << fileName;

    QFileInfo info(fileName);
    QDir dir = info.absoluteDir();

    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    a->spawnCompiler(program, argv, dir);
}

void EditVBoxLayout::onCompileFinished(int exitCode)
{
    qDebug() << "compile" << exitCode;
    runpushbutton.setEnabled(true);
}
