#ifndef GAMECONFIG_H
#define GAMECONFIG_H
#include "gameenum.h"
class QDataStream;
struct User{
  QString userName;//用户名
  QString imagePath;//用户头像路径
};

struct GameConfig{
    QString bgmPath;//背景音乐路径
    QString bgImagePath;//背景图片路径
    QString boardPath;//棋盘样式路径
    QString whiteChessPiecePath;//黑色棋子样式路径
    QString blackChessPiecePath;//白色棋子样式路径
    Game::Mode mode;//游戏模式，分双人、人机、机机、回想四种
    Game::AI whiteAIType;//白方AI等级
    Game::AI blackAIType;//黑方AI等级
    User whiteUser;//白方用户，一种级别的AI对应一个用户
    User blackUser;//黑方用户
    int enableUndoTimes;//允许后悔次数
    int enableCountTime;//每次下棋时倒计时的长度
    int AIDelay;//AI延迟，使AI下棋的时候表现更像人

};

QDataStream& operator <<(QDataStream& out,const GameConfig& gameConfig);
QDataStream& operator >>(QDataStream& in,GameConfig& gameConfig);

QDataStream& operator <<(QDataStream& out,const User& user);
QDataStream& operator >>(QDataStream& in,User& user);

#endif // GAMECONFIG_H
