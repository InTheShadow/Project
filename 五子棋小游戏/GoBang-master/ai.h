///////////////////////////////////////////////////////////////////////////
//主要实现：
//1.getAIOperation中emit AIOperated来AI操作
//2.根据不同的的type选择不同的AI算法，其中HighAI难度最高
///////////////////////////////////////////////////////////////
#ifndef AI_H
#define AI_H

#include <QObject>
#include <QPoint>
#include <QStack>
#include "gameenum.h"
#include "chessboard.h"
class AI : public QObject
{
    Q_OBJECT
public:
    explicit AI(QObject *parent = 0);
    ~AI();

    //初始化函数
    void initChessBoard();

    //参数设定函数
    void setType(Game::AI type = Game::NoAI) {this->type = type;}
    Game::AI getType(){return type;}
    void setPlayer(Game::Player player) {this->player = player;}

    //AI工作函数
    void translateOperation(Game::Operation operation,QPoint pos,Game::Player player);

    //AI悔棋相关函数
    void appendCoverPos(QPoint pos){coverPoses.push(pos);}
    int getRemainUndoTimes() const;
    void setRemainUndoTimes(int value);
signals:
    void AIOperated(Game::Operation operation, QPoint pos);
public slots:
    void getAIOperation();//该函数输出信号来返回AI操作
private:
     //数据成员
     Game::AI type;
     Game::Player player;
     int remainUndoTimes;
     QStack<QPoint> coverPoses;

     //对象成员
     ChessBoard* chessBoard;



};

#endif // AI_H
