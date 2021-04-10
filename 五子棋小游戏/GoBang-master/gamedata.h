#ifndef GAMEDATA_H
#define GAMEDATA_H
#include"gameenum.h"
#include <QPoint>
#include <QList>

struct Record{
    Game::Operation operation;//玩家操作
    Game::Player player;//执行玩家
    QPoint pos;//落子位置（仅落子操作有用）
    int remainCountTime;//剩余的计时器时间
};

struct GameData{
    Game::ChessPiece board[BOARDWIDTH][BOARDHEIGHT];//棋盘现状，每个点存在有黑棋子，有白棋子和什么都没有三态
    Game::Player currentPlayer;//当前正要操作的玩家
    int gameRound;//目前的回合数，用于判断是否可以投降
    int remainCountTime;//当前剩余的倒计时长度
    int whiteUndoTimes;//白方悔棋次数
    int blackUndoTimes;//黑方悔棋次数
    QList<Record> records;//记录从开始到结束的操作步骤
    QList<int> regret_records;//已经悔过的落子记录在记录中的位置，避免重复消除同一子
    bool whiteDeposited;//白方托管与否
    bool blackDeposited;//黑方托管与否
};

struct viewSettings{
    Game::Player player;//更新玩家
    bool regret_enable;//能否悔棋
    bool giveup_enable;//能够投降
    bool deposit_enable;//能否托管
    bool deposited;//当前是否托管
    int remainRegretTime;//剩余的悔棋次数
};

QDataStream& operator <<(QDataStream& out,const GameData& gameData);
QDataStream& operator >>(QDataStream& in,GameData& gameData);

QDataStream& operator <<(QDataStream& out, const Record& record);
QDataStream& operator >>(QDataStream& in,Record& record);


#endif // GAMEDATA_H
