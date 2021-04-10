#ifndef BLUEBUTTON_H
#define BLUEBUTTON_H
#include <QPushButton>
#include <QIcon>

class BlueButton : public QPushButton
{
    Q_OBJECT
public:
    explicit BlueButton(QWidget *parent = 0):QPushButton(parent)
    {
        this->setStyleSheet("QPushButton{border-image:url(:/images/blueButton.png);}\n");
    }

};

class BlueStartButton : public QPushButton
{
    Q_OBJECT
public:
    explicit BlueStartButton(QWidget *parent = 0):QPushButton(parent)
    {
        this->setStyleSheet("border-image:url(:/images/blueStartButton.png)");
    }

};


#endif // BLUEBUTTON_H
