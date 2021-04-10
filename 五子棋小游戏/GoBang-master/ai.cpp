#include "ai.h"

AI::AI(QObject *parent) : QObject(parent)
{
    chessBoard = NULL;
}

AI::~AI()
{
    delete chessBoard;
}

//获得AI操作
void AI::getAIOperation()
{
    Res* res = chessBoard->AI(type+1,-INF);
    if((!coverPoses.isEmpty()) && res->weight != BLACK_WIN ){
        QPoint result = coverPoses.pop();
        emit AIOperated(Game::Set,QPoint(result.x(),result.y()));
        return ;
    }
    if(res == NULL || res->weight == BLACK_WIN ) {
        if(remainUndoTimes > 0){
            remainUndoTimes--;
            emit AIOperated(Game::Regret,QPoint(0,0));
        }
        else{
            emit AIOperated(Game::GiveUp,QPoint(0,0));
        }
    }
    else emit AIOperated(Game::Set,QPoint(res->row,res->column));
}

//设置AI剩余悔棋次数
void AI::setRemainUndoTimes(int value)
{
    remainUndoTimes = value;
}

//初始化AI
void AI::initChessBoard()
{
    if(chessBoard != NULL) delete chessBoard;
    chessBoard = new ChessBoard;
    coverPoses.clear();
}

//AI翻译（记录）玩家的操作
void AI::translateOperation(Game::Operation operation, QPoint pos,Game::Player player)
{
    //define color
    int color;
    bool need_turn = false;
    if(this->player != Game::WhiteP) need_turn = true;
    if(need_turn && player == Game::WhiteP) color = BLACK;
    else if(need_turn && player == Game::BlackP) color = WHITE;
    else if((!need_turn) && player == Game::WhiteP) color = WHITE;
    else color = BLACK;
    if(operation == Game::Set){
        Chess chess = {pos.x(),pos.y(),color};
        chessBoard->putChess(chess);
    }

    else if(operation == Game::Regret){
        chessBoard->rollback(2);
    }

}




