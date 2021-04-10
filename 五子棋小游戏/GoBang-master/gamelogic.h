///////////////////////////////////////////////////////////////////////////
//主要实现：
//1.在适当的时机发出自定义的信号,一般在displayOperation和timeout中
//2.将AI对象中的自定义信号与槽连接，并用槽处理信号，得到AI输出结果
//3.针对不同的模式，对游戏逻辑进行修改
//4.使用AI前用AI的setAIType和setPlayer对其初始化
///////////////////////////////////////////////////////////////
#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>
#include "gamedata.h"
#include "ai.h"

class QTimer;
class GameLogic : public QObject
{
    Q_OBJECT
public:
    explicit GameLogic(QObject *parent = 0);
    ~GameLogic(){delete whiteAI;delete blackAI;}

    //开始游戏
    void startGame();

    //对游戏数据的操作
    void initGameData();
    void setGameData(GameData gameData) {this->gameData = gameData;}
    GameData& getGameData() {return gameData;}
    void setRecords(const QList<Record>& records){gameData.records = records;}

    //检验pos位置能否放棋子
    bool checkAddable(QPoint pos)const;

    //设置相关参数
    void setEnableUndoTimes(int times){enableUndoTimes = times;}
    void setEnableCountTime(int time){enableCountTime = time;}
    void setAIDelay(int time){AIDelay = time;}
    void setGameMode(Game::Mode gameMode){this->gameMode = gameMode;}
    void setAI(Game::AI whiteAIType = Game::NoAI,
               Game::AI blackAIType = Game::NoAI);


    //读取进度或换手时恢复状态
    void recoverAIState();
    void recoverBoardState();
    void recoverUIState();

    //用于托管功能
    void setOrCancelDeposit(Game::Player player);

    //用于非结束跳转
    void stopTimer();
signals:
    //游戏操作信号
    void eraseChessPiece(QPoint pos);//去除棋子信号（悔棋）
    void addChessPiece(QPoint pos,Game::Player player);//增加棋子信号（下棋）
    void gameOver(QString reason,Game::Player winner);//游戏结束信号，分正常结束、超时结束和投降结束三种

    //界面更新信号
    void updateTime(int remainCountTime,Game::Player player);//用于更新GameView中的计时器，一般用（QLCDNumber)
    void updateView(viewSettings settings);
public slots:
    void displayOperation(Game::Operation operation, QPoint pos);
    void timeout();
    void setGameSpeed(int value);
private:

    //数据成员
    int AIDelay;
    int enableUndoTimes;
    int enableCountTime;
    Game::Mode gameMode;
    GameData gameData;
    int gameSpeed;

    //对象成员
    AI* whiteAI;
    AI* blackAI;
    QTimer* countTimer;


    //回忆模式特有成员
    bool recallOverTime;
    QList<Record>::const_iterator record_iterator;

    //便利函数
    AI* getPlayerAI(Game::Player) const;
    int getPlayerUndoTimes(Game::Player) const;
    bool isPlayerWin(QPoint pos) const;
    void moveForwardRecord();

    //逻辑处理函数
    viewSettings setViewSettings(Game::Player player);
    QList<Record> findLastSetRecords(QList<Record>& records,int number);
    void deposit(Game::Player player,Game::AI AItype);
    void cancelDeposit(Game::Player);
    void waitForOperation();

};

#endif // GAMELOGIC_H
