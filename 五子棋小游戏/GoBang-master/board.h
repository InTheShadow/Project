#ifndef BOARD_H
#define BOARD_H

#include <QFrame>
#include "gameenum.h"

class Board : public QFrame
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = 0);
    void initBoard();
    void setWhiteChess(const QPixmap &value);
    void setBlackChess(const QPixmap &value);
    QPixmap getBoardChess() const;
    void setBoardChess(const QPixmap &value);
protected:
    void paintEvent(QPaintEvent *event);
public slots:
    void eraseChessPiece(QPoint pos);
    void addChessPiece(QPoint pos, Game::Player player);
private:
    QPixmap boardChess;
    QPixmap whiteChess;
    QPixmap blackChess;
    QPoint changeFromLogic(QPoint pos) const;

};

#endif // BOARD_H
