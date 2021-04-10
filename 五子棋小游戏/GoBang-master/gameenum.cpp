#include "gameenum.h"
#include<QObject>
#pragma execution_character_set("utf-8")
//得到对手玩家
Game::Player getOpponent(Game::Player player)
{
    return player == Game::WhiteP ? Game::BlackP : Game::WhiteP;
}

//获得游戏玩家文本
QString getPlayerStr(Game::Player player)
{
    return player == Game::WhiteP ? QObject::tr("白方") : QObject::tr("黑方");
}

//得到玩家所持棋子
Game::ChessPiece getChessPiece(Game::Player player)
{
    return player == Game::WhiteP ? Game::WhiteC : Game::BlackC;
}

//获得游戏模式文本
QString getModeStr(Game::Mode mode)
{
    if(mode == Game::PVP) return QObject::tr("双人对战");
    else if(mode == Game::PVC) return QObject::tr("人机对战");
    else if(mode == Game::CVC) return QObject::tr("机器对战");
    else return QObject::tr("回忆模式");
}
