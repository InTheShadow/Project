///////////////////////////////////////////////////////////////////////////
//主要实现：
//1.在适当的时机发出自定义的信号,
//2.将GameLogic对象中的自定义信号与槽连接，并用槽处理信号，绘制图形
//3.实现drawGameConfig来绘制游戏界面。loadGame读取文件，并调用GameLogic的setData赋值gameData.
//4.允许中途或结束时存盘，结束时的存盘记录用于回忆模式，中途的存盘可读取并继续游戏。游戏存盘内容包括GameConfig和GameData两块
//5.使用GameLogic前用一系列set函数和intiData（从头开始）或setData(读取进度)对GameLogic初始化。
//6.背景照片大小为800*600，棋盘大小为450*450,用户头像大小为100*100,棋子大小为30*30
///////////////////////////////////////////////////////////////

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QMainWindow>
#include "gameconfig.h"
#include "gamedata.h"
#pragma execution_character_set("utf-8")
class GameLogic;
class QGraphicsView;
class QMediaPlayer;
class Board;
namespace Ui {
class GameView;
}

class GameView : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameView(QWidget *parent = 0);
    ~GameView();

    //初始游戏界面
    void drawGameConfig(const GameConfig& gameconfig);//使用提供的GameConfig初始化界面
    //开始游戏
    void startGame();
signals:
    void backToStartWindow();//回到开始界面信号
protected:
    void mousePressEvent(QMouseEvent *event);
public slots:
    bool loadEndedGame(){return loadGame(true);}
    void saveEndedGame(){saveGame(true);}
    bool loadContinueGame(){return loadGame(false);}
    void saveContinueGame(){saveGame(false);}
private slots:
    //游戏逻辑连接槽
    void gameOver(QString reason, Game::Player winner);//游戏结束,分正常结束、超时结束和投降结束三种
    void updateTime(int time,Game::Player player);//时间更新，显示更新后的时间
    void updateView(viewSettings settings);
    void updateVolume(int value);

    //UI按钮槽
    void on_whiteRegret_clicked();
    void on_blackRegret_clicked();
    void on_whiteGiveup_clicked();
    void on_blackGiveup_clicked();
    void on_blackDeposit_clicked();
    void on_whiteDeposit_clicked();
    void on_actioReturn_triggered();
    void on_voiceCtrl_clicked();
private:
    //对象成员
    Ui::GameView *ui;
    GameLogic* gameLogic;
    Board* board;
    QMediaPlayer* player;

    //数据成员
    GameConfig gameConfig;

    //便利函数
    Game::AI getPlayerAI(Game::Player);
    QPoint changeToLogic(QPoint pos) const;

    //存取游戏函数
    bool loadGame(bool endedGame);
    bool saveGame(bool endedGame);

};

#endif // GAMEVIEW_H
