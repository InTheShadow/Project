#include "gamedata.h"
#include <QDataStream>

QDataStream &operator <<(QDataStream &out, const GameData &gameData)
{
    for(int i = 0;i<15;i++){
        for(int j = 0;j<15;j++){
            out << (int)gameData.board[i][j];
        }
    }
    out << (int)gameData.currentPlayer << gameData.gameRound
        << gameData.remainCountTime << gameData.whiteUndoTimes
        <<  gameData.blackUndoTimes<< gameData.records << gameData.regret_records
        << gameData.whiteDeposited << gameData.blackDeposited;
    return out;
}

QDataStream &operator >>(QDataStream &in, GameData &gameData)
{
    for(int i = 0;i<15;i++){
        for(int j = 0;j<15;j++){
            int temp;
            in >> temp;
            gameData.board[i][j] = Game::ChessPiece(temp);
        }
    }

    int currentPlayer;
    in  >> currentPlayer >> gameData.gameRound
        >> gameData.remainCountTime >> gameData.whiteUndoTimes
        >> gameData.blackUndoTimes >> gameData.records >> gameData.regret_records
        >> gameData.whiteDeposited >> gameData.blackDeposited;
    gameData.currentPlayer = Game::Player(currentPlayer);
    return in;
}

QDataStream &operator <<(QDataStream &out, const Record &record)
{
    out << (int)record.operation << (int)record.player
        << record.pos << record.remainCountTime;
    return out;
}

QDataStream &operator >>(QDataStream &in, Record &record)
{
    int operation,player;
    in >> operation >> player
        >> record.pos >> record.remainCountTime;
    record.operation = Game::Operation(operation);
    record.player = Game::Player(player);
    return in;
}


