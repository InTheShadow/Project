#include "gameconfig.h"
#include <QDataStream>


QDataStream &operator <<(QDataStream &out, const GameConfig &gameConfig)
{
    out << gameConfig.bgmPath << gameConfig.bgImagePath
        << gameConfig.boardPath << gameConfig.whiteChessPiecePath
        << gameConfig.blackChessPiecePath
        << (int)gameConfig.mode << (int)gameConfig.whiteAIType
        << (int)gameConfig.blackAIType  << gameConfig.whiteUser
        << gameConfig.blackUser << gameConfig.enableUndoTimes
        << gameConfig.enableCountTime << gameConfig.AIDelay;

    return out;
}

QDataStream &operator >>(QDataStream &in, GameConfig &gameConfig)
{
    int whiteAIType,blackAIType,mode;
    in >> gameConfig.bgmPath >> gameConfig.bgImagePath
        >> gameConfig.boardPath >> gameConfig.whiteChessPiecePath
        >> gameConfig.blackChessPiecePath
        >> mode >> whiteAIType
        >> blackAIType  >> gameConfig.whiteUser
        >> gameConfig.blackUser>> gameConfig.enableUndoTimes
        >> gameConfig.enableCountTime >> gameConfig.AIDelay;
    gameConfig.whiteAIType = Game::AI(whiteAIType);
    gameConfig.blackAIType = Game::AI(blackAIType);
    gameConfig.mode = Game::Mode(mode);
    return in;
}

QDataStream &operator <<(QDataStream &out, const User &user)
{
    out << user.userName << user.imagePath;
    return out;
}

QDataStream &operator >>(QDataStream &in, User &user)
{
    in >> user.userName >> user.imagePath;
    return in;
}
