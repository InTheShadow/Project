#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QFileDialog>
#include <QFile>
#pragma execution_character_set("utf-8")
SettingWindow::SettingWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingWindow)
{

    ui->setupUi(this);
    setWindowTitle(tr("五子棋设定"));
    connect(ui->AIDelay,SIGNAL(valueChanged(int)),this,SLOT(AIDelayChanged(int)));
    connect(ui->countTime,SIGNAL(valueChanged(int)),this,SLOT(countTimeChanged(int)));
    connect(ui->undoTimes,SIGNAL(valueChanged(int)),this,SLOT(undoTimesChanged(int)));
}

SettingWindow::~SettingWindow()
{
    delete ui;
}

//初始化设定变更界面
void SettingWindow::createContents()
{
    ui->musicPath->setText(gameConfigPtr->bgmPath);
    ui->bgiPath->setText(gameConfigPtr->bgImagePath);
    ui->blackPath->setText(gameConfigPtr->blackChessPiecePath);
    ui->whitePath->setText(gameConfigPtr->whiteChessPiecePath);
    ui->AIDelay->setValue(gameConfigPtr->AIDelay);
    ui->AIDelayEnter->setText(QString("%1").arg(gameConfigPtr->AIDelay));
    ui->countTimeEnter->setText(QString("%1").arg(gameConfigPtr->enableCountTime));
    ui->undoTimesEnter->setText(QString("%1").arg(gameConfigPtr->enableUndoTimes));
}

//返回主界面按钮
void SettingWindow::on_returnBtn_clicked()
{
    emit backFromSettings();
}

//初始化设定按钮
void SettingWindow::on_defaultBtn_clicked()
{
    gameConfigPtr->bgmPath = DATAPATH+"music/bgm/default_bgm.mp3";
    gameConfigPtr->bgImagePath = ":/images/default_bgi.png";
    gameConfigPtr->boardPath = ":/images/default_board.png";
    gameConfigPtr->whiteChessPiecePath = ":/images/default_whiteChess.png";
    gameConfigPtr->blackChessPiecePath = ":/images/default_blackChess.png";
    gameConfigPtr->enableUndoTimes = 3;
    gameConfigPtr->enableCountTime = 60;
    gameConfigPtr->AIDelay  = 3;
    createContents();
}

//音乐路径发现按钮
void SettingWindow::on_musicBtn_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(this, tr("读取背景音乐"),DATAPATH+"music/bgm/",tr("Music(*.mp3))"));
    if (filePath.isEmpty())
        return ;
    ui->musicPath->setText(filePath);
}

//更新设定中背景音乐路径
void SettingWindow::on_musicPath_textChanged(const QString &arg1)
{
    gameConfigPtr->bgmPath = arg1;
}

//背景图片发现按钮
void SettingWindow::on_bgiBtn_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(this, tr("读取背景图片"),DATAPATH+"images/background/",tr("Images(*.png))"));
    if (filePath.isEmpty())
        return ;
    ui->bgiPath->setText(filePath);
}

//更新设定中背景图片路径
void SettingWindow::on_bgiPath_textChanged(const QString &arg1)
{
    gameConfigPtr->bgImagePath = arg1;
}

//白方棋子样式寻找按钮
void SettingWindow::on_whiteBtn_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(this, tr("读取白棋样式"),DATAPATH+"images/whiteChess/",tr("Images(*.png))"));
    if (filePath.isEmpty())
        return ;
    ui->whitePath->setText(filePath);
}

//更新设定中白方棋子样式
void SettingWindow::on_whitePath_textChanged(const QString &arg1)
{
    gameConfigPtr->whiteChessPiecePath = arg1;
}

//黑方棋子样式寻找按钮
void SettingWindow::on_blackBtn_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(this, tr("读取黑棋样式"),DATAPATH+"images/blackChess/",tr("Images(*.png))"));
    if (filePath.isEmpty())
        return ;
    ui->blackPath->setText(filePath);
}

//更新设定中黑方棋子样式
void SettingWindow::on_blackPath_textChanged(const QString &arg1)
{
    gameConfigPtr->blackChessPiecePath = arg1;
}

//更新设定中悔棋次数
void SettingWindow::on_undoTimesEnter_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    if(value < 0) value = 0;
    if(value > 10) value = 10;
    ui->undoTimes->setValue(value);
}

//悔棋次数设定文本框
void SettingWindow::undoTimesChanged(int value)
{
    gameConfigPtr->enableUndoTimes = value;
    ui->undoTimesEnter->setText(tr("%1").arg(value));
}

//更新设定中游戏倒计时
void SettingWindow::countTimeChanged(int value)
{
    gameConfigPtr->enableCountTime = value;
    ui->countTimeEnter->setText(tr("%1").arg(value));
}

//更新设定中AI延时
void SettingWindow::AIDelayChanged(int value)
{
    gameConfigPtr->AIDelay = value;
    ui->AIDelayEnter->setText(tr("%1").arg(value));
}

//游戏倒计时设定文本框
void SettingWindow::on_countTimeEnter_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    if(value < 10) value = 10;
    if(value > 60) value = 60;
    ui->countTime->setValue(value);
}

//AI延时设定文本框
void SettingWindow::on_AIDelayEnter_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    if(value < 0) value = 0;
    if(value > 10) value = 10;
    ui->AIDelay->setValue(value);
}
