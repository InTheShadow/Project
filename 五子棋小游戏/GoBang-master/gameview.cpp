#include "gameview.h"
#include "ui_gameview.h"
#include "gamelogic.h"
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QSound>
#include <QFileInfo>
#include <QMouseEvent>
#include <QPainter>
#include <QSlider>
#include "board.h"
#pragma execution_character_set("utf-8")


const QString WIN_SOUND=":/sounds/win.wav";
const QString LOSE_SOUND=":/sounds/lose.wav";

GameView::GameView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameView)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("五子棋游戏"));
    gameLogic = new GameLogic;
    player = new QMediaPlayer(this);
    board = new Board(this);

    setMaximumSize(800,600);
    setMinimumSize(800,600);
    connect(gameLogic,SIGNAL(addChessPiece(QPoint,Game::Player)),board,SLOT(addChessPiece(QPoint,Game::Player)));
    connect(gameLogic,SIGNAL(eraseChessPiece(QPoint)),board,SLOT(eraseChessPiece(QPoint)));
    connect(gameLogic,SIGNAL(gameOver(QString,Game::Player)),this,SLOT(gameOver(QString,Game::Player)));
    connect(gameLogic,SIGNAL(updateTime(int,Game::Player)),this,SLOT(updateTime(int,Game::Player)));
    connect(gameLogic,SIGNAL(updateView(viewSettings)),this,SLOT(updateView(viewSettings)));
    connect(ui->recallSlider,SIGNAL(valueChanged(int)),gameLogic,SLOT(setGameSpeed(int)));
    connect(ui->voiceSlider,SIGNAL(valueChanged(int)),this,SLOT(updateVolume(int)));

    connect(ui->actionLoad,SIGNAL(triggered(bool)),this,SLOT(loadContinueGame()));
    connect(ui->actionSave,SIGNAL(triggered(bool)),this,SLOT(saveContinueGame()));
    connect(ui->actionQuit,SIGNAL(triggered(bool)),qApp,SLOT(quit()));

}

GameView::~GameView()
{
    delete ui;
    delete gameLogic;
}

//根据游戏设定初始化游戏逻辑和游戏界面
void GameView::drawGameConfig(const GameConfig &gameConfig)
{
    //初始化游戏逻辑
    this->gameConfig = gameConfig;
    gameLogic->setEnableUndoTimes(gameConfig.enableUndoTimes);
    gameLogic->setEnableCountTime(gameConfig.enableCountTime);
    gameLogic->setAIDelay(gameConfig.AIDelay);
    gameLogic->setGameMode(gameConfig.mode);
    gameLogic->setAI(gameConfig.whiteAIType,gameConfig.blackAIType);
    gameLogic->initGameData();

    //绘制游戏主界面UI
    this->setObjectName("gameView");
    setStyleSheet(QString("QMainWindow#gameView{border-image:url(%1);}").arg(gameConfig.bgImagePath));
    ui->whiteCount->display(gameConfig.enableCountTime);
    ui->blackCount->display(gameConfig.enableCountTime);
    ui->gameMode->setText(getModeStr(gameConfig.mode));
    ui->whiteUser->setText(gameConfig.whiteUser.userName);
    ui->blackUser->setText(gameConfig.blackUser.userName);
    ui->whiteImage->setPixmap(QPixmap(gameConfig.whiteUser.imagePath));
    ui->blackImage->setPixmap(QPixmap(gameConfig.blackUser.imagePath));
    ui->whiteGiveup->setEnabled(false);
    ui->whiteRegret->setEnabled(false);
    ui->whiteRegret->setText(tr("悔棋(剩余%1)").arg(gameConfig.enableUndoTimes));
    ui->blackRegret->setText(tr("悔棋(剩余%1)").arg(gameConfig.enableUndoTimes));
    ui->blackRegret->setEnabled(false);
    ui->whiteDeposit->setEnabled(false);
    ui->whiteDeposit->setText(tr("托管"));
    ui->blackDeposit->setEnabled("false");
    ui->blackDeposit->setText(tr("托管"));
    ui->blackGiveup->setEnabled(false);
    ui->recallSlider->setValue(1);
    if(gameConfig.mode != Game::Recall){
        ui->recallLabel->hide();
        ui->recallSlider->hide();
        ui->actionLoad->setEnabled(true);
        ui->actionSave->setEnabled(true);
    }
    else{
        ui->recallLabel->show();
        ui->recallSlider->show();
        ui->actionLoad->setEnabled(false);
        ui->actionSave->setEnabled(false);
    }
    QStyle* style = QApplication::style();
    QIcon icon = style->standardIcon(QStyle::SP_MediaVolume);
    ui->voiceCtrl->setIcon(icon);
    ui->whiteChess->setPixmap(QPixmap(gameConfig.whiteChessPiecePath).scaled(16,16));
    ui->blackChess->setPixmap(QPixmap(gameConfig.blackChessPiecePath).scaled(16,16));
    //绘制和初始化游戏中心棋盘
    board->setObjectName("bd");
    board->move(160,70);
    board->resize(450,450);
    board->setLineWidth(0);
    board->setStyleSheet(QString("Board#bd{border-image:url(%1);}").arg(gameConfig.boardPath));
    board->setWhiteChess(QPixmap(gameConfig.whiteChessPiecePath));
    board->setBlackChess(QPixmap(gameConfig.blackChessPiecePath));
    board->initBoard();

    //播放音乐
    QMediaPlaylist* playlist = new QMediaPlaylist;
    playlist->addMedia(QUrl::fromLocalFile(gameConfig.bgmPath));
    playlist->setCurrentIndex(0);
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    player->setPlaylist(playlist);
    player->setVolume(50);
    ui->voiceSlider->setValue(50);
    ui->voiceNum->setText(tr("%1").arg(50));
    ui->voiceSlider->hide();
    ui->voiceNum->hide();

}


//开始游戏
void GameView::startGame()
{
    player->play();
    gameLogic->startGame();
}


void GameView::mousePressEvent(QMouseEvent *event)
{
    //点击在棋盘中心来落子
    QRect board_border = board->geometry();
    if(event->x() >= board_border.left()
            && event->y() >= board_border.top()
            && event->x() <= board_border.right()
            && event->y() <= board_border.bottom()){
        QPoint vPos = changeToLogic(event->pos());

        //检查是否可以落子
        if(gameLogic->checkAddable(vPos)){
            gameLogic->displayOperation(Game::Set,vPos);
        }
    }

    //点击音量控制条外部使其隐藏
    QRect voice_border = ui->voiceSlider->geometry();
    if(event->x() <= voice_border.left()
            || event->y() <= voice_border.top()
            || event->x() >= voice_border.right()
            || event->y() >= voice_border.bottom()){
        ui->voiceSlider->hide();
        ui->voiceNum->hide();
    }

    QMainWindow::mousePressEvent(event);
}


//游戏结束
void GameView::gameOver(QString reason, Game::Player winner)
{
    //播放游戏结束音效
    player->stop();
    if(gameLogic->getGameData().currentPlayer != winner)
        QSound::play(LOSE_SOUND);
    else QSound::play(WIN_SOUND);

    //初始化对话框
    QMessageBox box;
    box.setIcon(QMessageBox::Question);
    QPushButton *yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
    box.addButton(tr("否(&N)"),QMessageBox::NoRole);

    //回忆模式考虑是否再回忆一遍
    if(gameConfig.mode == Game::Recall){
        box.setWindowTitle(tr("回放结束"));
        box.setText(reason+"\n"+tr("是否再回忆一遍？选否的话会回到开始界面"));
        box.exec();

        if(box.clickedButton() == yesBtn) {
            QList<Record> records = gameLogic->getGameData().records;
            drawGameConfig(gameConfig);
            gameLogic->setRecords(records);
            startGame();
        }
        else emit backToStartWindow();
        return;
    }

    //正常模式保存棋局弹出
    box.setWindowTitle(tr("保存游戏"));
    box.setText(reason+"\n"+tr("是否保存游戏过程？"));
    box.exec();
    if(box.clickedButton() == yesBtn) saveEndedGame();

    //正常模式考虑是否再来一局
    box.setWindowTitle(tr("棋局结束"));
    box.setText(tr("是否再来一局？选否的话会回到开始界面"));
    box.exec();
    if(box.clickedButton() == yesBtn) {
        drawGameConfig(gameConfig);
        startGame();
    }
    else emit backToStartWindow();

}

//更新倒计时
void GameView::updateTime(int time, Game::Player player)
{
    if(player == Game::WhiteP) ui->whiteCount->display(time);
    else ui->blackCount->display(time);
}

//更新界面的按钮和头像
void GameView::updateView(viewSettings settings)
{
    if(settings.player == Game::WhiteP){
        ui->whiteRegret->setEnabled(settings.regret_enable);
        ui->whiteRegret->setText(tr("悔棋(剩余%1)").arg(settings.remainRegretTime));
        ui->whiteGiveup->setEnabled(settings.giveup_enable);
        ui->whiteDeposit->setEnabled(settings.deposit_enable);
        if(settings.deposited){
            ui->whiteImage->setPixmap(QPixmap(":/images/whiteRobot"));
            ui->whiteDeposit->setText(tr("取消托管"));
        }
        else{
            ui->whiteImage->setPixmap(QPixmap(gameConfig.whiteUser.imagePath));
            ui->whiteDeposit->setText(tr("托管"));
        }
    }
    else {
        ui->blackRegret->setEnabled(settings.regret_enable);
        ui->blackRegret->setText(tr("悔棋(剩余%1)").arg(settings.remainRegretTime));
        ui->blackGiveup->setEnabled(settings.giveup_enable);
        ui->blackDeposit->setEnabled(settings.deposit_enable);
        if(settings.deposited){
            ui->blackImage->setPixmap(QPixmap(":/images/blackRobot"));
            ui->blackDeposit->setText(tr("取消托管"));
        }
        else{
            ui->blackImage->setPixmap(QPixmap(gameConfig.blackUser.imagePath));
            ui->blackDeposit->setText(tr("托管"));
        }
    }
}

//更新背景音乐的音量
void GameView::updateVolume(int value)
{
    QStyle* style = QApplication::style();
    QIcon icon;
    if(value == 0) icon = style->standardIcon(QStyle::SP_MediaVolumeMuted);
    else icon = style->standardIcon(QStyle::SP_MediaVolume);
    ui->voiceCtrl->setIcon(icon);
    ui->voiceNum->setText(tr("%1").arg(value));
    player->setVolume(value);
}

//白色悔棋
void GameView::on_whiteRegret_clicked()
{
    gameLogic->displayOperation(Game::Regret,QPoint(0,0));
}

//黑方悔棋
void GameView::on_blackRegret_clicked()
{
    gameLogic->displayOperation(Game::Regret,QPoint(0,0));
}

//白方投降
void GameView::on_whiteGiveup_clicked()
{
    gameLogic->displayOperation(Game::GiveUp,QPoint(0,0));
}

//黑方投降
void GameView::on_blackGiveup_clicked()
{
    gameLogic->displayOperation(Game::GiveUp,QPoint(0,0));
}

//得到玩家AI类型
Game::AI GameView::getPlayerAI(Game::Player player)
{
    if(player == Game::WhiteP) return gameConfig.whiteAIType;
    else return gameConfig.blackAIType;
}

//将主界面中的物理位置转化为棋子在15*15棋盘落下的逻辑位置
QPoint GameView::changeToLogic(QPoint pos) const
{
    QPoint result;
    result.setX((pos.x()-board->geometry().left())/30);
    result.setY((pos.y()-board->geometry().top())/30);
    return result;
}

//存储游戏（包括中间游戏进度和最终棋局）
bool GameView::saveGame(bool endedGame)
{
    QString filePath;
    if(endedGame)
           filePath = QFileDialog::getSaveFileName(this, tr("保存棋局"),DATAPATH+"savedata/",tr("Savedata(*.esd)"));
    else filePath = QFileDialog::getSaveFileName(this, tr("保存进度"),DATAPATH+"savedata/",tr("Savedata(*.sd)"));
    if (filePath.isEmpty())
        return false;
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly|QFile::Truncate)) {
        QMessageBox::warning(this, tr("无法保存进度"),
                             tr("无法保存进度 %1:\n%2.")
                             .arg(filePath).arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);

    out << gameConfig << gameLogic->getGameData();
    file.close();
    return true;
}

//读取游戏（包括中间游戏进度和最终棋局）
bool GameView::loadGame(bool endedGame)
{
    QString filePath;
    if(endedGame)
        filePath = QFileDialog::getOpenFileName(this, tr("读取棋局"),DATAPATH+"savedata/",tr("Savedata(*.esd)"));
    else filePath = QFileDialog::getOpenFileName(this, tr("读取进度"),DATAPATH+"savedata/",tr("Savedata(*.sd)"));
    if (filePath.isEmpty())
        return false;
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("无法读取进度"),
                             tr("无法读取进度 %1:\n%2.")
                             .arg(filePath).arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    GameData gameData;
    in >> gameConfig >> gameData;
    file.close();

    //从当前进度开始游戏或者回忆模式从头开始回忆棋局
    if(endedGame) {
        gameConfig.mode = Game::Recall;
     }
    drawGameConfig(gameConfig);
    if(endedGame){
        gameLogic->setRecords(gameData.records);
    }
    else {
        gameLogic->setGameData(gameData);
        gameLogic->recoverAIState();
        gameLogic->recoverBoardState();
        gameLogic->recoverUIState();
    }

    if(!endedGame)startGame();
    return true;


}

//黑方托管或解除托管
void GameView::on_blackDeposit_clicked()
{
    gameLogic->setOrCancelDeposit(Game::BlackP);
}

//白方托管或解除托管
void GameView::on_whiteDeposit_clicked()
{
   gameLogic->setOrCancelDeposit(Game::WhiteP);

}

//返回游戏开始界面
void GameView::on_actioReturn_triggered()
{
    player->stop();
    gameLogic->stopTimer();
    emit backToStartWindow();
}

//显示或隐藏音量控制条
void GameView::on_voiceCtrl_clicked()
{
    if(ui->voiceSlider->isHidden()){
        ui->voiceSlider->show();
        ui->voiceNum->show();
    }
    else {
        ui->voiceSlider->hide();
        ui->voiceNum->hide();
    }
}
