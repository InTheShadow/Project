///////////////////////////////////////////////////////////////////////////
//主要实现：
//1.使用GameView前调用其drawGameConfig或loadGameByPath或loadGame初始化界面，切换界面后调用startGame开始游戏
//2.建立对话框可以选择游戏模式和用户，允许创建新用户或载入上次使用过的用户，进入游戏时载入用户列表
//3.建立主界面和设定界面，进入主界面时载入设定，设定界面允许修改设定，退出设定界面时保存设定（设定见gameconfig.h)
//4.将GameView对象中的自定义信号与槽连接，并用槽处理信号
//5.背景照片大小为800*600，棋盘大小为450*450,用户头像大小为100*100,棋子大小为30*30
///////////////////////////////////////////////////////////////

#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>

#include "gameconfig.h"
class GameView;
class SettingWindow;
class aboutDialog;
namespace Ui {
class StartWindow;
}
class QMediaPlayer;

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = 0);
    ~StartWindow();
public slots:
    void backToStartWindow();//回到主界面
    void backFromSettings();
private slots:
    void on_startButton_clicked();
    void on_whiteChange_clicked();
    void on_blackChange_clicked();
    void on_exitButton_clicked();
    void on_whiteUser_currentTextChanged(const QString &arg1);
    void on_blackUser_currentTextChanged(const QString &arg1);
    void on_whiteNew_clicked();
    void on_blackNew_clicked();
    void on_gameStart_clicked();
    void on_mode_currentIndexChanged(int index);
    void on_whiteAILevel_currentIndexChanged(int index);
    void on_blackAILevel_currentIndexChanged(int index);
    void on_quitButton_clicked();
    void on_settingButton_clicked();
    void on_loadButton_clicked();
    void on_recallButton_clicked();
    void on_aboutButton_clicked();

private:
    //数据成员
    GameConfig gameConfig;
    QList<User> userList;
    int blackUserIndex;
    int whiteUserIndex;
    int blackPlayerIndex;
    int whitePlayerIndex;
    User whiteTempUser;
    User blackTempUser;

    //对象成员
    Ui::StartWindow *ui;
    GameView* gameView;
    SettingWindow* settingWindow;
    QMediaPlayer* player;
    aboutDialog* about;

    //便利函数
    int findByUserName(const QString& userName);
    QString getUserImagePath(Game::Player player) const;
    QString getUserName(Game::Player player);
    void setOrCancelSpecialUser();

    //存取函数
    bool loadConfig();
    bool saveConfig();
    bool loadUserList();
    bool saveUserList();
    void initConfig();
    void initStartWindow();
    QString loadImage();
    void startGame();
    void setWidgetsExceptAI(Game::Player player,bool able);
    void updateUserWidget();
};

#endif // STARTWINDOW_H
