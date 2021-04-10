#include "gamelogic.h"
#include <QTimer>
#include <QtAlgorithms>
#include <QSound>
#pragma execution_character_set("utf-8")
const QString CHESS_ONE_SOUND=":sounds/chessone.wav";
const QString REGRET_SOUND=":sounds/regret.wav";
const QString COUNT_SOUND=":sounds/count.wav";

GameLogic::GameLogic(QObject *parent) : QObject(parent)
{
    whiteAI = new AI();
    connect(whiteAI,SIGNAL(AIOperated(Game::Operation,QPoint)),this,SLOT(displayOperation(Game::Operation,QPoint)));
    blackAI = new AI();
    connect(blackAI,SIGNAL(AIOperated(Game::Operation,QPoint)),this,SLOT(displayOperation(Game::Operation,QPoint)));
    countTimer = new QTimer(this);
    connect(countTimer,SIGNAL(timeout()),this,SLOT(timeout()));

}

//初始化游戏数据
void GameLogic::initGameData()
{
    gameData.blackUndoTimes = 0;
    gameData.whiteUndoTimes = 0;
    gameData.currentPlayer = Game::WhiteP;
    gameData.gameRound = 0;
    gameData.records.clear();
    gameData.regret_records.clear();
    gameData.whiteDeposited = false;
    gameData.blackDeposited = false;
    for(int i = 0;i<15;i++)
        for(int j = 0;j<15;j++)
            gameData.board[i][j] = Game::NoC;
    gameSpeed = 1000;
}

//判断某逻辑位置是否可以落子
bool GameLogic::checkAddable(QPoint pos) const
{
    return gameData.board[pos.x()][pos.y()] == Game::NoC &&
            getPlayerAI(gameData.currentPlayer)->getType() == Game::NoAI;

}

//设置AI
void GameLogic::setAI(Game::AI whiteAIType, Game::AI blackAIType)
{
    whiteAI->setPlayer(Game::WhiteP);
    whiteAI->setType(whiteAIType);
    if(whiteAIType != Game::NoAI) {
        whiteAI->initChessBoard();
        whiteAI->setRemainUndoTimes(enableUndoTimes);
    }
    blackAI->setPlayer(Game::BlackP);
    blackAI->setType(blackAIType);
    if(blackAIType != Game::NoAI) {
        blackAI->initChessBoard();
        blackAI->setRemainUndoTimes(enableUndoTimes);
    }
}

//读取进度或者托管后根据游戏数据恢复AI的状态
void GameLogic::recoverAIState()
{
    //初始化AI存储数据
    if(gameData.whiteDeposited) whiteAI->setType(Game::LowAI);
    if(gameData.blackDeposited) blackAI->setType(Game::LowAI);
    if(whiteAI->getType() != Game::NoAI) {
        whiteAI->initChessBoard();
        whiteAI->setRemainUndoTimes(gameData.whiteUndoTimes);
    }
    if(blackAI->getType() != Game::NoAI) {
        blackAI->initChessBoard();
        whiteAI->setRemainUndoTimes(gameData.blackUndoTimes);
    }

    //根据记录恢复数据
    Record temp;
    for(int i = 0;i<=gameData.records.size()-1;i++){
         temp = gameData.records[i];
         if(whiteAI->getType() != Game::NoAI)
             whiteAI->translateOperation(temp.operation,temp.pos,temp.player);
         if(blackAI->getType() != Game::NoAI)
             blackAI->translateOperation(temp.operation,temp.pos,temp.player);
    }
}

//读取进度后根据游戏数据恢复棋盘的状态
void GameLogic::recoverBoardState()
{

    //根据记录恢复数据
    for(int i = 0;i<BOARDWIDTH;i++){
        for(int j = 0;j < BOARDHEIGHT;j++){
            if(gameData.board[i][j] == Game::BlackC)
                emit addChessPiece(QPoint(i,j),Game::BlackP);
            else if(gameData.board[i][j] == Game::WhiteC)
                emit addChessPiece(QPoint(i,j),Game::WhiteP);
        }
    }
}

//读取进度后根据游戏数据恢复主界面（棋盘以外）的状态
void GameLogic::recoverUIState()
{
    viewSettings settings = setViewSettings(gameData.currentPlayer);
    emit updateView(settings);
    settings = setViewSettings(getOpponent(gameData.currentPlayer));
    emit updateView(settings);
}

//根据情况使玩家托管和解除托管
void GameLogic::setOrCancelDeposit(Game::Player player)
{
    if(gameMode != Game::Recall){
        Record record = {Game::Deposit,player,QPoint(0,0),gameData.remainCountTime};
        gameData.records.append(record);
    }

    if(player == Game::WhiteP){
        if(!gameData.whiteDeposited) deposit(player,Game::LowAI);
        else cancelDeposit(player);
    }
    else{
        if(!gameData.blackDeposited) deposit(player,Game::LowAI);
        else cancelDeposit(player);
    }
}

//游戏中途结束时停止时钟
void GameLogic::stopTimer()
{
    countTimer->stop();
}

//玩家托管
void GameLogic::deposit(Game::Player player, Game::AI AIType)
{

    if(player == Game::WhiteP) {
        whiteAI->setType(AIType);
        gameData.whiteDeposited = true;
    }
    else{
        blackAI->setType(AIType);
        gameData.blackDeposited = true;
    }
    if(gameMode != Game::Recall)recoverAIState();
    emit updateView(setViewSettings(player));
}

//玩家解除托管
void GameLogic::cancelDeposit(Game::Player player)
{
    if(player == Game::WhiteP) {
        whiteAI->setType(Game::NoAI);
        gameData.whiteDeposited = false;
    }
    else{
        blackAI->setType(Game::NoAI);
        gameData.blackDeposited = false;
    }
    emit updateView(setViewSettings(player));
}

//根据玩家或AI的游戏操作调整游戏主界面和游戏数据
void GameLogic::displayOperation(Game::Operation operation, QPoint pos)
{
    countTimer->stop();


    //输出信号
    if(operation == Game::Set) {
        QSound::play(CHESS_ONE_SOUND);
        gameData.board[pos.x()][pos.y()] = getChessPiece(gameData.currentPlayer);
        emit addChessPiece(pos,gameData.currentPlayer);
    }
    else if(operation == Game::Regret) {
        QSound::play(REGRET_SOUND);
        QList<Record> result = findLastSetRecords(gameData.records,2);
        if(gameData.currentPlayer == Game::WhiteP) gameData.whiteUndoTimes++;
        else gameData.blackUndoTimes++;
        gameData.board[result.at(0).pos.x()][result.at(0).pos.y()] = Game::NoC;
        gameData.board[result.at(1).pos.x()][result.at(1).pos.y()] = Game::NoC;
        if(getPlayerAI(gameData.currentPlayer)->getType() != Game::NoAI){
            getPlayerAI(gameData.currentPlayer)->appendCoverPos(result.at(0).pos);
        }
        if(result.size()>=2){
            emit eraseChessPiece(result.at(0).pos);
            emit eraseChessPiece(result.at(1).pos);
        }
    }

    //记录信息
    if(gameMode != Game::Recall){
        Record record = {operation,gameData.currentPlayer,pos,gameData.remainCountTime};
        gameData.records.append(record);
    }

    //判断游戏是否结束
    if(operation == Game::GiveUp){
        emit gameOver(getPlayerStr(gameData.currentPlayer)+tr("投降！"),getOpponent(gameData.currentPlayer));
        return;
    }
    if(operation == Game::Set && isPlayerWin(pos)) {
        emit gameOver(getPlayerStr(gameData.currentPlayer)+tr("获胜！"),gameData.currentPlayer);
        return;
    }

    //修正当前游戏轮数
    if(operation == Game::Regret) gameData.gameRound--;
    else if(operation == Game::Set && gameData.currentPlayer == Game::BlackP) gameData.gameRound++;


    //修正剩余悔棋次数
    gameData.remainCountTime = enableCountTime;
    emit updateTime(gameData.remainCountTime,gameData.currentPlayer);

    //后续处理和切换玩家
    if(gameMode == Game::Recall) moveForwardRecord();
    else{
        if(whiteAI->getType() != Game::NoAI) whiteAI->translateOperation(operation,pos,gameData.currentPlayer);
        if(blackAI->getType() != Game::NoAI) blackAI->translateOperation(operation,pos,gameData.currentPlayer);
    }
    gameData.currentPlayer = (operation == Game::Regret)?gameData.currentPlayer:getOpponent(gameData.currentPlayer);

    //等待玩家或AI的操作
    waitForOperation();
}

//计时器经过一秒后的操作
void GameLogic::timeout()
{
    //更新时间
    gameData.remainCountTime--;
    if(gameData.remainCountTime == 3){
        QSound::play(COUNT_SOUND);
    }
    emit updateTime(gameData.remainCountTime,gameData.currentPlayer);

    //回忆模式
    if(gameMode == Game::Recall){
        if(gameData.remainCountTime <= 0){
            countTimer->stop();
            emit gameOver(getPlayerStr(gameData.currentPlayer)+tr("超时判负!"),getOpponent(gameData.currentPlayer));
        }
        else if((!recallOverTime) && gameData.remainCountTime <= record_iterator->remainCountTime){
            if(record_iterator->operation == Game::Deposit) {
                setOrCancelDeposit(record_iterator->player);
                moveForwardRecord();
            }
            else displayOperation(record_iterator->operation,record_iterator->pos);
        }
    }

    //正常模式
    else{
        //玩家等待操作
        if(getPlayerAI(gameData.currentPlayer)->getType() == Game::NoAI){
            if(gameData.remainCountTime <= 0){
                countTimer->stop();
                emit gameOver(getPlayerStr(gameData.currentPlayer)+tr("超时判负!"),getOpponent(gameData.currentPlayer));
            }
        }
        //AI延迟一定时间后执行操作
        else{
            if(enableCountTime-gameData.remainCountTime >= AIDelay) {
                getPlayerAI(gameData.currentPlayer)->getAIOperation();
            }
        }
    }
}

//设置游戏速度（仅回忆模式下用于调节回忆速度）
void GameLogic::setGameSpeed(int value)
{
    gameSpeed = 1000/value;
    countTimer->stop();
    countTimer->start(gameSpeed);
}

//得到玩家AI
AI *GameLogic::getPlayerAI(Game::Player player) const
{
    if(player == Game::WhiteP) return whiteAI;
    else return blackAI;
}

//得到玩家悔棋次数
int GameLogic::getPlayerUndoTimes(Game::Player player) const
{
    if(player == Game::WhiteP) return gameData.whiteUndoTimes;
    else return gameData.blackUndoTimes;
}

//检验当前玩家是否获胜
bool GameLogic::isPlayerWin(QPoint pos) const
{
    int i;
    int xMin = qMax(pos.x()-4,0);
    int xMax = qMin(pos.x()+4,BOARDWIDTH-1);
    int yMin = qMax(pos.y()-4,0);
    int yMax = qMin(pos.y()+4,BOARDHEIGHT-1);
    int leftup = qMin(pos.x()-xMin,pos.y()-yMin);
    int leftdown = qMin(pos.x()-xMin,yMax-pos.y());
    int rightup = qMin(xMax-pos.x(),pos.y()-yMin);
    int rightdown = qMin(xMax-pos.x(),yMax-pos.y());

    //横向检验
    int count = 0;
    for(i = xMin;i<=xMax;i++) {
        if(gameData.board[i][pos.y()] == getChessPiece(gameData.currentPlayer))
        {
            count++;
            if(count >=5) return true;
        }
        else count = 0;
    }
    if(count >= 5) return true;

    //纵向检验
    count = 0;
    for(i = yMin;i<=yMax;i++) {
        if(gameData.board[pos.x()][i] == getChessPiece(gameData.currentPlayer))
        {
            count++;
            if(count >=5) return true;
        }
        else count = 0;
    }
    if(count >= 5) return true;

    //左上至右下检验
    count = 0;
    for(i=-leftup;i<=rightdown;i++){
        if(gameData.board[pos.x()+i][pos.y()+i] == getChessPiece(gameData.currentPlayer))
        {
            count++;
            if(count >=5) return true;
        }
        else count = 0;
    }


    //左下至右上检验
    count = 0;
    for(i=-leftdown;i<=rightup;i++){
        if(gameData.board[pos.x()+i][pos.y()-i] == getChessPiece(gameData.currentPlayer))
        {
            count++;
            if(count >=5) return true;
        }
        else count = 0;
    }

    return false;
}

//设置主界面（除棋盘）的设定信息
viewSettings GameLogic::setViewSettings(Game::Player player)
{
    //初始化界面设定
    viewSettings settings;
    settings.player = player;
    settings.deposited = false;
    settings.deposit_enable = false;
    settings.giveup_enable = false;
    settings.regret_enable = false;
    settings.remainRegretTime = enableUndoTimes-getPlayerUndoTimes(player);
    //是否委托设置
    if(player == Game::WhiteP) settings.deposited = gameData.whiteDeposited;
    else settings.deposited = gameData.blackDeposited;

    //回忆模式设置
    if(gameMode == Game::Recall) return settings;

    //AI角色设置
    if(getPlayerAI(player)->getType() != Game::NoAI && settings.deposited){
        settings.deposit_enable = true;
        return settings;
    }
    else if(getPlayerAI(player)->getType() == Game::NoAI) settings.deposit_enable = true;
    else return settings;

    //非当前玩家设置
    if(gameData.currentPlayer != player) return settings;

    //人角色设置
    if(getPlayerUndoTimes(player) < enableUndoTimes && gameData.gameRound >= 1 )
        settings.regret_enable = true;
    if(gameData.gameRound >=  10) settings.giveup_enable = true;
    return settings;

}

//找到最近未被悔棋过的number条落子记录
QList<Record> GameLogic::findLastSetRecords(QList<Record> &records, int number)
{
    if(records.size() < number) return QList<Record>();
    QList<Record> result;
    int max;
    if(gameMode == Game::Recall) max = record_iterator-records.begin();
    else max = records.length()-1;
    for(int i = max;i>=0;i--){
        if(records.at(i).operation == Game::Set && (!gameData.regret_records.contains(i))) {
            result.append(records.at(i));
            gameData.regret_records.append(i);
        }
        if(result.size() >= number) break;
    }
    return result;
}

//等待AI或玩家操作
void GameLogic::waitForOperation()
{
    gameData.remainCountTime = enableCountTime;
    viewSettings settings = setViewSettings(gameData.currentPlayer);
    emit updateView(settings);
    settings = setViewSettings(getOpponent(gameData.currentPlayer));
    emit updateView(settings);
    countTimer->start(gameSpeed);
}

//开始游戏
void GameLogic::startGame()
{
   if(gameMode == Game::Recall) {
       recallOverTime = false;
       if(gameData.records.size() == 0)
           recallOverTime = true;
       else record_iterator = gameData.records.begin();
   }
   waitForOperation();
}

//回忆模式下前移当前记录
void GameLogic::moveForwardRecord()
{
    if(record_iterator >= gameData.records.end()){
        recallOverTime = true;
    }
    else{
        record_iterator++;
    }
}
