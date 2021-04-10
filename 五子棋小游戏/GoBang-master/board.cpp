#include "board.h"
#include <QPainter>
#include <QPixmap>

Board::Board(QWidget *parent):QFrame(parent)
{

}
//初始化棋盘（主要是棋子分布图）
void Board::initBoard()
{
    boardChess = QPixmap(450,450);
    boardChess.fill(Qt::transparent);
}

void Board::paintEvent(QPaintEvent * event)
{
    //在棋盘上绘制棋子分布图
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(QPoint(0,0),boardChess);
}

//棋子分布图中擦除棋子并更新
void Board::eraseChessPiece(QPoint pos)
{
    QPainter painter;
    painter.begin(&boardChess);
    QRect rect;
    rect.setTopLeft(changeFromLogic(pos));
    rect.setBottomRight(changeFromLogic(pos)+QPoint(30,30));
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect,Qt::transparent);
    painter.end();
    update();

}

//棋子分布图中更新棋子
void Board::addChessPiece(QPoint pos, Game::Player player)
{

    QPainter painter;
    painter.begin(&boardChess);
    if(player == Game::WhiteP)
        painter.drawPixmap(changeFromLogic(pos),whiteChess);
    else painter.drawPixmap(changeFromLogic(pos),blackChess);
    painter.end();
    update();
}

//设置棋子分布图
void Board::setBoardChess(const QPixmap &value)
{
    boardChess = value;
    update();
}

//获得棋子分布图
QPixmap Board::getBoardChess() const
{
    return boardChess;
}

//设置黑色棋子样式
void Board::setBlackChess(const QPixmap &value)
{
    blackChess = value;
}

//设置白色棋子样式
void Board::setWhiteChess(const QPixmap &value)
{
    whiteChess = value;
}

//将棋子在15*15棋盘落下的逻辑位置转化为棋盘中的物理位置
QPoint Board::changeFromLogic(QPoint pos) const
{
    QPoint result;
    result.setX(pos.x()*30);
    result.setY(pos.y()*30);
    return result;
}
