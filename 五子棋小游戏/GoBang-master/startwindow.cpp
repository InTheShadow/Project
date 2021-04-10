#include "startwindow.h"
#include "ui_startwindow.h"
#include "gameview.h"
#include "settingwindow.h"
#include "aboutdialog.h"
#include <QFile>
#include <QMessageBox>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QFileDialog>

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow)
{

    ui->setupUi(this);
    setWindowTitle(tr("五子棋"));
    gameView = new GameView;
    player = new QMediaPlayer;
    settingWindow = new SettingWindow;
    about = new aboutDialog;
    setMaximumSize(800,600);
    setMinimumSize(800,600);
    connect(gameView,SIGNAL(backToStartWindow()),this,SLOT(backToStartWindow()));
    connect(settingWindow,SIGNAL(backFromSettings()),this,SLOT(backFromSettings()));
    loadUserList();
    loadConfig();
    initStartWindow();
    settingWindow->setGameConfigPtr(&gameConfig);
    player->play();
}

StartWindow::~StartWindow()
{
    delete ui;
    delete gameView;
}

//处理主界面回到开始界面信号
void StartWindow::backToStartWindow()
{
    player->play();
    gameView->hide();
    this->show();
}

//处理设定界面回到开始界面信号
void StartWindow::backFromSettings()
{
    settingWindow->hide();
    this->show();
    saveConfig();
}

//读取游戏设定
bool StartWindow::loadConfig()
{
    QString fileName = DATAPATH+"config.cfg";
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        initConfig();
        return false;
    }

    QDataStream in(&file);
    in >> gameConfig;
    file.close();
    return true;
}

//存储游戏设定
bool StartWindow::saveConfig()
{
    QString fileName = DATAPATH+"config.cfg";
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("无法保存设定"),
                             tr("无法保存设定 %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out << gameConfig;
    file.close();
    return true;
}

//读取用户名单
bool StartWindow::loadUserList()
{

    userList.clear();
    User LowAI = {tr("初级电脑"),QString(":/images/LowAI.png")};
    User MidAI = {tr("中级电脑"),QString(":/images/MidAI.png")};
    User HighAI = {tr("高级电脑"),QString(":/images/HighAI.png")};
    User default_black = {tr("黑方玩家"),QString(":/images/default_black.png")};
    User default_white = {tr("白方玩家"),QString(":/images/default_white.png")};
    User unknown = {tr("未知"),QString(":images/unknown.png")};
    userList << LowAI << MidAI << HighAI;
    userList <<  default_white << default_black << unknown;
    //增加电脑用户和默认用户
    QList<User> tempList;
    QString fileName = DATAPATH+"userList.ul";
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }

    QDataStream in(&file);
    in >> tempList;
    userList.append(tempList);
    file.close();
    return true;
}

//存储用户名单
bool StartWindow::saveUserList()
{
    QList<User> tempList;
    if(userList.length() > 6) tempList = userList.mid(6);
    else return false;
    //去除开始的三个电脑角色和三个默认角色，不用存
    QString fileName = DATAPATH+"userList.ul";
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly|QFile::Truncate)) {
        QMessageBox::warning(this, tr("无法保存用户列表"),
                             tr("无法保存用户列表 %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out << tempList;
    file.close();
    return true;
}

//初始化游戏设定
void StartWindow::initConfig()
{
    gameConfig.bgmPath = DATAPATH+"music/bgm/default_bgm.mp3";
    gameConfig.bgImagePath = ":/images/default_bgi.png";
    gameConfig.boardPath = ":/images/default_board.png";
    gameConfig.whiteChessPiecePath = ":/images/default_whiteChess.png";
    gameConfig.blackChessPiecePath = ":/images/default_blackChess.png";
    gameConfig.mode = Game::PVP;
    gameConfig.whiteAIType = Game::NoAI;
    gameConfig.blackAIType = Game::NoAI;
    gameConfig.whiteUser = userList.at(3);
    gameConfig.blackUser = userList.at(4);
    gameConfig.enableUndoTimes = 3;
    gameConfig.enableCountTime = 60;
    gameConfig.AIDelay  = 3;
}

//初始化开始界面
void StartWindow::initStartWindow()
{
    QMediaPlaylist* playlist = new QMediaPlaylist;
    playlist->addMedia(QUrl::fromLocalFile(DATAPATH+"music/bgm/start_bgm.mp3"));
    playlist->setCurrentIndex(0);
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    player->setPlaylist(playlist);
    ui->userWidget->hide();
    about->hide();
    player->setVolume(50);

}

//读取头像
QString StartWindow::loadImage()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(this, tr("读取头像"),DATAPATH+"images/avater/",tr("Images(*.png))"));
    if (filePath.isEmpty())
        return NULL;
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("无法读取头像"),
                             tr("无法读取头像 %1:\n%2.")
                             .arg(filePath).arg(file.errorString()));
        return NULL;
    }
    file.close();
    return filePath;
}

//开始游戏
void StartWindow::startGame()
{
    player->stop();
    if(gameConfig.mode == Game::Recall){
        if(gameView->loadEndedGame()){
           this->hide();
           gameView->show();
           gameView->startGame();
        }
        else player->play();
    }
    else{
        gameView->drawGameConfig(gameConfig);
        this->hide();
        gameView->show();
        gameView->startGame();
    }
}

//设置用户设定窗口中除AI类型之外的enble属性
void StartWindow::setWidgetsExceptAI(Game::Player player,bool able)
{

   if(player == Game::WhiteP){
       ui->whiteUser->setEnabled(able);
       ui->whiteChange->setEnabled(able);
       ui->whiteNew->setEnabled(able);
   }

   else{
       ui->blackUser->setEnabled(able);
       ui->blackChange->setEnabled(able);
       ui->blackNew->setEnabled(able);
   }
}

//更新用户窗口
void StartWindow::updateUserWidget()
{


    ui->whiteUser->setCurrentText(getUserName(Game::WhiteP));

    ui->whiteImage->clear();
    ui->whiteImage->setPixmap(QPixmap(getUserImagePath(Game::WhiteP)));
    ui->blackUser->setCurrentText(getUserName(Game::BlackP));
    ui->blackImage->clear();
    ui->blackImage->setPixmap(QPixmap(getUserImagePath(Game::BlackP)));

    ui->whiteAILevel->setCurrentIndex((int)gameConfig.whiteAIType);
    ui->blackAILevel->setCurrentIndex((int)gameConfig.blackAIType);

    if(gameConfig.mode == Game::Recall) {
        ui->mode->setCurrentIndex(1);
        ui->whiteAILevel->setEnabled(false);
        setWidgetsExceptAI(Game::WhiteP,false);
        ui->blackAILevel->setEnabled(false);
        setWidgetsExceptAI(Game::BlackP,false);
    }
    else{
        ui->mode->setCurrentIndex(0);
        ui->whiteAILevel->setEnabled(true);
        ui->blackAILevel->setEnabled(true);
        if(gameConfig.whiteAIType != Game::NoAI) setWidgetsExceptAI(Game::WhiteP,false);
        else setWidgetsExceptAI(Game::WhiteP,true);
        if(gameConfig.blackAIType != Game::NoAI) setWidgetsExceptAI(Game::BlackP,false);
        else setWidgetsExceptAI(Game::BlackP,true);
    }
}


//初始化并显示用户设定窗口
void StartWindow::on_startButton_clicked()
{
    //初始化游戏数据
    whiteUserIndex = findByUserName(gameConfig.whiteUser.userName);
    blackUserIndex = findByUserName(gameConfig.blackUser.userName);
    if(whiteUserIndex < 3 || whiteUserIndex == 5) whitePlayerIndex = 3;
    else whitePlayerIndex = whiteUserIndex;
    if(blackUserIndex < 3 || blackUserIndex == 5) blackPlayerIndex = 4;
    else blackPlayerIndex = blackUserIndex;

    //初始化用户列表与用户数据
    ui->mode->clear();
    QStringList userNameList;
    for(int i = 6;i<userList.length();i++){
        userNameList << userList.at(i).userName;

    }
    ui->whiteUser->setEditable(true);
    ui->whiteUser->clear();
    ui->whiteUser->addItems(userNameList);
    ui->blackUser->setEditable(true);
    ui->blackUser->clear();
    ui->blackUser->addItems(userNameList);
    ui->whiteChess->clear();
    ui->whiteChess->setPixmap(QPixmap(gameConfig.whiteChessPiecePath).scaled(15,15));
    ui->blackChess->clear();
    ui->blackChess->setPixmap(QPixmap(gameConfig.blackChessPiecePath).scaled(15,15));


    //初始化AI等级列表与AI情况
    QStringList AILevelList;
    AILevelList << tr("无AI") << tr("低级AI") << tr("中级AI") << tr("高级AI");
    ui->whiteAILevel->setEditable(false);
    ui->whiteAILevel->clear();
    ui->whiteAILevel->addItems(AILevelList);
    ui->blackAILevel->setEditable(false);
    ui->blackAILevel->clear();
    ui->blackAILevel->addItems(AILevelList);

    //初始化模式列表
    QStringList modeList;
    ui->mode->setEditable(false);
    modeList << tr("对战模式") <<  tr("回忆模式");
    ui->mode->addItems(modeList);


    //更新界面
    setOrCancelSpecialUser();
    updateUserWidget();
    ui->userWidget->show();

}

//白方修改头像
void StartWindow::on_whiteChange_clicked()
{
    QString imagePath = loadImage();
    if(imagePath != NULL){
        if(whiteUserIndex == -1){
            whiteTempUser.imagePath = imagePath;
        }
        else {
            userList[whiteUserIndex].imagePath = imagePath;
        }
    }
    updateUserWidget();
}

//黑方修改头像
void StartWindow::on_blackChange_clicked()
{
    QString imagePath = loadImage();
    if(imagePath != NULL){
        if(blackUserIndex == -1){
            blackTempUser.imagePath = imagePath;
        }
        else {
            userList[blackUserIndex].imagePath = imagePath;
        }
    }
    updateUserWidget();
}

//退出（隐藏）用户设定窗口
void StartWindow::on_exitButton_clicked()
{
    if(whiteUserIndex != -1) gameConfig.whiteUser = userList.at(whiteUserIndex);
    if(blackUserIndex != -1) gameConfig.blackUser = userList.at(blackUserIndex);
    saveUserList();
    saveConfig();
    ui->userWidget->hide();
}

//更改白方用户名
void StartWindow::on_whiteUser_currentTextChanged(const QString &arg1)
{
    int index = findByUserName(arg1);

    if(index == -1){
        whiteTempUser.userName = arg1;
        whiteTempUser.imagePath = getUserImagePath(Game::WhiteP);
    }
    whiteUserIndex = index;
    if(gameConfig.mode != Game::Recall && gameConfig.whiteAIType == Game::NoAI) whitePlayerIndex = whiteUserIndex;

    updateUserWidget();
}

//更改黑方用户名
int StartWindow::findByUserName(const QString &userName)
{
    for(int i = 0;i<userList.size();i++){
        if(userList.at(i).userName == userName) return i;
    }
    return -1;

}

//获得用户设定界面中用户头像路径
QString StartWindow::getUserImagePath(Game::Player player)const
{
    if(player == Game::WhiteP) {
        if(whiteUserIndex == -1) return whiteTempUser.imagePath;
        else return userList.at(whiteUserIndex).imagePath;
    }
    else{
        if(blackUserIndex == -1) return blackTempUser.imagePath;
        else return userList.at(blackUserIndex).imagePath;
    }

}

//获得用户设定界面中用户名
QString StartWindow::getUserName(Game::Player player)
{
    if(player == Game::WhiteP) {
        if(whiteUserIndex == -1) return whiteTempUser.userName;
        else return userList.at(whiteUserIndex).userName;
    }
    else{
        if(blackUserIndex == -1) return blackTempUser.userName;
        else return userList.at(blackUserIndex).userName;
    }
}

//设置或消除特殊的用户（如电脑用户和未知用户）
void StartWindow::setOrCancelSpecialUser()
{
    if(gameConfig.mode == Game::Recall){
        whiteUserIndex = 5;
        blackUserIndex = 5;
        return ;
    }
    if(gameConfig.whiteAIType == Game::NoAI) whiteUserIndex = whitePlayerIndex;
    else whiteUserIndex = int(gameConfig.whiteAIType-1);
    if(gameConfig.blackAIType == Game::NoAI) blackUserIndex = blackPlayerIndex;
    else blackUserIndex = int(gameConfig.blackAIType-1);
}

//更改黑方用户名
void StartWindow::on_blackUser_currentTextChanged(const QString &arg1)
{
    int index = findByUserName(arg1);

    if(index == -1){
        blackTempUser.imagePath = getUserImagePath(Game::BlackP);
        blackTempUser.userName = arg1;
    }
    blackUserIndex = index;
    if(gameConfig.mode != Game::Recall && gameConfig.blackAIType == Game::NoAI) blackPlayerIndex = blackUserIndex;
    updateUserWidget();
}

//白方新建用户
void StartWindow::on_whiteNew_clicked()
{
    if(whiteUserIndex == -1){
        userList.append(whiteTempUser);
        whiteUserIndex = userList.size()-1;
        whitePlayerIndex = whiteUserIndex;
        ui->blackUser->insertItem(ui->blackUser->count(),ui->whiteUser->currentText());
        ui->whiteUser->insertItem(ui->whiteUser->count(),ui->whiteUser->currentText());
    }
    else{
        QMessageBox::warning(this, tr("无法创建新用户"),
                             tr("用户名已存在"));
    }
    updateUserWidget();
}

//黑方新建用户
void StartWindow::on_blackNew_clicked()
{
    if(blackUserIndex == -1){
        userList.append(blackTempUser);
        blackUserIndex = userList.size()-1;
        blackPlayerIndex = blackUserIndex;
        ui->blackUser->insertItem(ui->blackUser->count(),ui->blackUser->currentText());
        ui->whiteUser->insertItem(ui->whiteUser->count(),ui->blackUser->currentText());
    }
    else{
        QMessageBox::warning(this, tr("无法创建新用户"),
                             tr("用户名已存在，请修改用户名！"));
    }
    updateUserWidget();
}

//关闭用户设定界面并开始游戏
void StartWindow::on_gameStart_clicked()
{
    if(whiteUserIndex == -1 || blackUserIndex == -1){
        QMessageBox::warning(this, tr("用户不存在"),
                             tr("不存在当前用户，请先创建用户!"));
        return;
    }
    gameConfig.whiteUser = userList.at(whiteUserIndex);
    gameConfig.blackUser = userList.at(blackUserIndex);

    saveUserList();
    saveConfig();
    ui->userWidget->hide();
    startGame();
}

//更改游戏模式
void StartWindow::on_mode_currentIndexChanged(int index)
{
    //双人对战
    if(index < 0) return;
    if(index == 1) {
        gameConfig.mode = Game::Recall;
    }
    else{
        //设置游戏模式
        if(gameConfig.whiteAIType == Game::NoAI && gameConfig.blackAIType == Game::NoAI)
        {
            gameConfig.mode = Game::PVP;
        }
        else if(gameConfig.whiteAIType != Game::NoAI && gameConfig.blackAIType != Game::NoAI)
        {
            gameConfig.mode = Game::CVC;
        }
        else gameConfig.mode = Game::PVC;
    }

    setOrCancelSpecialUser();
    updateUserWidget();

}

//更改白方AI等级
void StartWindow::on_whiteAILevel_currentIndexChanged(int index)
{
    if(index < 0) return;
    gameConfig.whiteAIType = (Game::AI)index;
    setOrCancelSpecialUser();
    updateUserWidget();
}

//更改黑方AI等级
void StartWindow::on_blackAILevel_currentIndexChanged(int index)
{
    if(index < 0) return;
    gameConfig.blackAIType = (Game::AI)index;
    setOrCancelSpecialUser();
    updateUserWidget();
}

//退出游戏按钮
void StartWindow::on_quitButton_clicked()
{
    qApp->quit();
}

//设定变更按钮
void StartWindow::on_settingButton_clicked()
{
    settingWindow->createContents();
    this->hide();
    settingWindow->show();
}

void StartWindow::on_loadButton_clicked()
{
    player->stop();
    if(gameView->loadContinueGame()){
        this->hide();
        gameView->show();
    }
    else player->play();
}

void StartWindow::on_recallButton_clicked()
{
    player->stop();
    gameConfig.mode = Game::Recall;
    if(gameView->loadEndedGame()){
        gameView->startGame();
        this->hide();
        gameView->show();
    }
    else player->play();
}

void StartWindow::on_aboutButton_clicked()
{
    about->show();
}
