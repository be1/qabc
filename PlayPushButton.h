#ifndef PLAYPUSHBUTTON_H
#define PLAYPUSHBUTTON_H

#include <QPushButton>

#define TEXT_PLAY "Play"
#define TEXT_STOP "Stop"

class PlayPushButton: public QPushButton
{
	Q_OBJECT

public:
	PlayPushButton(QWidget* parent = nullptr);
    ~PlayPushButton();
    void flip();
    bool isPlay();

private:
    bool play;
};
#endif
