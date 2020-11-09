#ifndef EDITVBOXLAYOUT_H
#define EDITVBOXLAYOUT_H

#include "RunPushButton.h"
#include "PlayPushButton.h"
#include "AbcPlainTextEdit.h"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>

class EditVBoxLayout: public QVBoxLayout
{
	Q_OBJECT

public:
    explicit EditVBoxLayout(const QString& fileName, QWidget* parent = nullptr);
	~EditVBoxLayout();

    AbcPlainTextEdit *abcPlainTextEdit();
    PlayPushButton *playPushButton();
    RunPushButton *runPushButton();
    void setFileName(const QString& fn);

protected slots:
    void onXChanged(int value);
    void onPlayClicked();
    void onRunClicked();

    void onPlayFinished(int exitCode);
    void onSynthFinished(int exitCode);
    void onCompileFinished(int exitCode);

private:
	AbcPlainTextEdit abcplaintextedit;
    PlayPushButton playpushbutton;
    RunPushButton runpushbutton;
    QHBoxLayout hboxlayout;
    QSpinBox xspinbox;
    QLabel xlabel;
    QString fileName;
};
#endif
