#ifndef GAMEENUM_H
#define GAMEENUM_H
#include <QString>
#include <QDebug>
const int BOARDWIDTH = 15;
const int BOARDHEIGHT = 15;
const QString DATAPATH = "./";
//调试用路径，Release版时需修改
namespace Game {
   enum Player{WhiteP,BlackP};
   enum ChessPiece{NoC,WhiteC,BlackC};
   enum Operation{Set,Regret,GiveUp,Deposit};//分别对应落棋,悔棋，投降，托管
   enum Mode{PVP,PVC,CVC,Recall};//分别对应人人，人机，双机，回想
   enum AI{NoAI,LowAI,MidAI,HighAI};
}

Game::Player getOpponent(Game::Player player) ;
QString getPlayerStr(Game::Player player);
Game::ChessPiece getChessPiece(Game::Player player);
QString getModeStr(Game::Mode);

#endif // GAMEENUM_H
