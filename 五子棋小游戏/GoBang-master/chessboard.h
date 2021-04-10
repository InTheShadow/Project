
#ifndef _CHESSBOARD_H_
#define _CHESSBOARD_H_

#include <cstdlib>
#include <vector>
#include <algorithm>
#include <list>
#include <ctime>

#define BOARDSIZE 15
#define NONE 0
#define BLACK 1
#define WHITE 2
#define BLACK_WIN 100000
#define WHITE_WIN (-100000)
#define FIVE_W 100000
#define FOUR_W 5000
#define THREE_W 1000
#define TWO_W 200
#define ONE_W 10
#define MAX 500
#define INF 0x7fffffff

typedef struct _Chess{
    int row;
    int column;
    int color; //1 is black, 2 is white

public:
    _Chess() {}
    _Chess(int r, int c, int color):row(r), column(c), color(color){}
}Chess;

typedef struct _Res {
    int weight;
    int row;
    int column;

public:
    _Res() {}
    _Res(int w, int r, int c):weight(w), row(r), column(c){}
}Res;

typedef std::pair<int, int> Point;

class ChessBoard {
private:
    //int board[BOARDSIZE][BOARDSIZE]; //棋盘
    bool win[BOARDSIZE][BOARDSIZE][MAX];   //赢法
    int winCount; //赢法数
    int (*blackWin)[2];   //黑棋(人)每一种赢法达成棋子数
    int (*whiteWin)[2];   //白棋(AI)每一种赢法达成的棋子数
    bool is_end;     //判断游戏是否结束
    std::list<Chess> chessStep; //list存储每一步棋
    int enemyAlpha;
    //std::vector<Point> nearPoints;
public:
    ChessBoard();
    int board[BOARDSIZE][BOARDSIZE];
    ~ChessBoard();
    void putChess(Chess &chess); //下棋
    void rollback(int n); //退回n步棋
    std::vector<Point> getNearPoint(Chess &chess); //得到一个棋子周围的位置
    bool isValid(Point &point); //判断当前点是否符合要求
    std::vector<Point> possiblePoint(); //得到下一步棋可行的位置
    int evaluate();   //评估当前棋盘的值，越大对人越有利，越小对AI越有利
    bool isBlackWin();  //判断黑棋是否获胜
    bool isWhiteWin();  //判断白棋是否获胜
    bool isEmemyWin(){return enemyAlpha == BLACK_WIN;}
    void end(); //游戏结束
    bool isEnd(); //返回游戏结束标志
    Res* Person(int depth, int beta); //人下棋
    Res* AI(int depth, int alpha); //AI下棋
};

#endif
