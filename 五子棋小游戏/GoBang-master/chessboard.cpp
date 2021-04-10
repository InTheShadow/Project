#include "chessboard.h"
#include "gameenum.h"
#include <QtAlgorithms>
using namespace std;

ChessBoard::ChessBoard():is_end(false), winCount(0){
    memset(board, 0, sizeof(int)*BOARDSIZE*BOARDSIZE); //初始化棋盘
    enemyAlpha = 0;
    memset(win,false,sizeof(bool)*BOARDSIZE*BOARDSIZE*MAX);
    //横向赢法
    for(int i=0; i<BOARDSIZE;++i) {
        for(int j=0; j<BOARDSIZE-5; ++j) {
            for(int k=0; k<5; ++k) {
                win[i][j+k][winCount] = true;
            }
            winCount ++;
        }
    }

    //纵向赢法
    for(int i=0; i<BOARDSIZE; ++i) {
        for(int j=0; j<BOARDSIZE-5; ++j) {
            for(int k=0; k<5; ++k) {
                win[j+k][i][winCount] = true;
            }
            winCount ++;
        }
    }

    //左上到右下赢法
    for(int i=0; i<BOARDSIZE-5; ++i) {
        for(int j=0; j<BOARDSIZE-5; ++j) {
            for(int k=0; k<5; ++k) {
                win[i+k][j+k][winCount] = true;
            }
            winCount ++;
        }
    }

    //左下到右上赢法
    for(int i=BOARDSIZE-1; i>=4; --i) {
        for(int j=0; j<BOARDSIZE-5; ++j) {
            for(int k=0; k<5; ++k) {
                win[i-k][j+k][winCount] = true;
            }
            winCount ++;
        }
    }

    //初始化白棋和黑棋每一种赢法达成棋子数
    int *_blackWin = new int[winCount * 2];
    int *_whiteWin = new int[winCount * 2];
    blackWin = (int(*)[2])_blackWin;
    whiteWin = (int(*)[2])_whiteWin;
    memset((int*)blackWin, 0, sizeof(int)*winCount*2);
    memset((int*)whiteWin, 0, sizeof(int)*winCount*2);
}

ChessBoard::~ChessBoard() {
    delete [] (int*)blackWin;
    delete [] (int*)whiteWin;
    //清空堆栈
    chessStep.clear();
}

void ChessBoard::putChess(Chess &chess) {
    int row = chess.row;
    int column = chess.column;
    int color = chess.color;
    if(board[row][column] == NONE) {
        board[row][column] = color;
        //记录
        chessStep.push_back(chess);
    }

    //下棋之后对每一种赢法进行更新
    for(int i=0; i<winCount; ++i) {
        if(win[row][column][i]) {
            if(color == BLACK) {
                blackWin[i][BLACK - 1] ++;
                whiteWin[i][BLACK - 1] ++;
            } else {
                whiteWin[i][WHITE - 1] ++;
                blackWin[i][WHITE - 1] ++;
            }
        }
    }

    //棋盘已经下满，游戏结束
    if(chessStep.size() == BOARDSIZE*BOARDSIZE) {
        end();
    }
}

//退回N步棋
void ChessBoard::rollback(int n) {
    Chess chess;
    for(int i=0; i<n; i++) {
        if(!chessStep.empty()) {
            chess = chessStep.back();
            chessStep.pop_back();
            //得到上一步棋，弹出堆栈
        } else {
            return;
        }

        int row = chess.row;
        int column = chess.column;
        int color = chess.color;

        //悔棋(当前位置置空)
        board[row][column] = NONE;

        //更新赢法
        for(int j=0; j<winCount; ++j) {
            if(win[row][column][j]) {
                if(color == BLACK) {
                    blackWin[j][BLACK - 1] --;
                    whiteWin[j][BLACK - 1] --;
                }
                else {
                    whiteWin[j][WHITE - 1] --;
                    blackWin[j][WHITE - 1] --;
                }
            }
        }
    }
    is_end = false; //如果上一步棋造成游戏结束，则悔棋后游戏继续
}

vector<Point> ChessBoard::getNearPoint(Chess &chess) {
    std::vector<Point> nearPoints;
    int row, column;

    for(int i=-2; i<=2; ++i) {
        //左上到右上
        row = chess.row + i;
        column = chess.column + i;
        Point point1(row, column);
        if(isValid(point1)) {
            nearPoints.push_back(point1);
        }

        //左下到右上
        row = chess.row - i;
        column = chess.column + i;
        Point point2(row, column);
        if(isValid(point2)) {
            nearPoints.push_back(point2);
        }

        //行
        row = chess.row;
        column = chess.column + i;
        Point point3(row, column);
        if(isValid(point3)) {
            nearPoints.push_back(point3);
        }

        //列
        row = chess.row + i;
        column = chess.column;
        Point point4(row, column);
        if(isValid(point4)) {
            nearPoints.push_back(point4);
        }
    }
    return nearPoints;
}

bool ChessBoard::isValid(Point &point) {
    int row = point.first;
    int column = point.second;
    return row >= 0 && row < BOARDSIZE && column >= 0 && column < BOARDSIZE && board[row][column] == NONE;
}

vector<Point> ChessBoard::possiblePoint() {
    vector<Point> possibleSteps;
    int Len = chessStep.size(); //堆栈的长度
    //棋盘正中位置
    int centerRow = BOARDSIZE/2;
    int centerColumn = BOARDSIZE/2;

    //还没下棋或者只下过一步棋，看正中位置是否有棋子
    if(Len==0 || (Len==1 && board[centerRow][centerColumn]==NONE)) {
        Point point(centerRow, centerColumn);
        possibleSteps.push_back(point);
        //正中位置无棋子
    } else {
        //正中位置有棋子，随机得到周围的一个位置
        if(Len == 1) {
            srand(unsigned(time(NULL)));
            int row = centerRow + ((rand()%2)?-1:1);
            int column = centerColumn + ((rand()%2)?-1:1);
            Point point(row,column);
            possibleSteps.push_back(point);
        } else {
            bool visited[BOARDSIZE][BOARDSIZE];
            memset(visited, false, sizeof(bool)*BOARDSIZE*BOARDSIZE);
            //对list中的每一步棋的得到附近点
            for (list<Chess>::iterator it1 = chessStep.begin(); it1 != chessStep.end(); ++it1) {
                vector<Point> nearPoints = getNearPoint(*it1);
                //对每一个附近点判断是否可行
                for (vector<Point>::iterator it2 = nearPoints.begin(); it2 != nearPoints.end(); ++it2) {
                    int row = it2->first;
                    int column = it2->second;
                    Point point(row, column);
                    if (!visited[row][column]) {
                        possibleSteps.push_back(point);
                        visited[row][column] = true;
                    }
                }
            }
        }
    }
    return possibleSteps;
}

int ChessBoard::evaluate() {
    int blackW, whiteW;
    //blackW为黑子权值，越大对自己越有利； whiteW为白子权值，越小对自己越有利
    int black[5] = { 0 };
    int white[5] = { 0 };
    //黑白子各种1-5长度达成个数
    blackW = whiteW = 0;

    for(int i=0; i<winCount; ++i) {
        if(blackWin[i][BLACK-1] == 5 && !blackWin[i][WHITE-1]) {
            return BLACK_WIN;
        }
        if (whiteWin[i][WHITE-1] == 5 && !whiteWin[i][BLACK-1]) {
            return WHITE_WIN;
        }
        if (blackWin[i][BLACK - 1] == 4 && !blackWin[i][WHITE - 1]) {
            black[4] ++;
        }
        if (whiteWin[i][WHITE - 1] == 4 && !whiteWin[i][BLACK - 1]) {
            white[4] ++;
        }
        if (blackWin[i][BLACK - 1] == 3 && !blackWin[i][WHITE - 1]) {
            black[3] ++;
        }
        if (whiteWin[i][WHITE - 1] == 3 && !whiteWin[i][BLACK - 1]) {
            white[3] ++;
        }
        if (blackWin[i][BLACK - 1] == 2 && !blackWin[i][WHITE - 1]) {
            black[2] ++;
        }
        if (whiteWin[i][WHITE - 1] == 2 && !whiteWin[i][BLACK - 1]) {
            white[2] ++;
        }
        if (blackWin[i][BLACK - 1] == 1 && !blackWin[i][WHITE - 1]) {
            black[1] ++;
        }
        if (whiteWin[i][WHITE - 1] == 4 && !whiteWin[i][BLACK - 1]) {
            white[1] ++;
        }
    }
    blackW = black[4] * FOUR_W + black[3] * THREE_W + black[2] * TWO_W + black[1] * ONE_W;
    whiteW = white[4] * FOUR_W + white[3] * THREE_W + white[2] * TWO_W + white[1] * ONE_W;
    //计算黑白子权重
    return blackW - whiteW;
}

bool ChessBoard::isBlackWin() {
    int weight = evaluate();
    return weight == BLACK_WIN;
}

bool ChessBoard::isWhiteWin() {
    int weight = evaluate();
    return weight == WHITE_WIN;
}

void ChessBoard::end() {
    is_end = true;
}

bool ChessBoard::isEnd() {
    return is_end;
}

Res* ChessBoard::Person(int depth, int beta) {
    int row, column, alpha;
    alpha = -INF;
    if (depth == 0) {
        alpha = evaluate();
        Res* res = new Res(alpha, 0, 0);
        return res;
    }
    else {
        vector<Point> steps = possiblePoint();
        if (steps.size()) {
            for (int i = 0; i < steps.size(); ++i) {
                Point step = steps[i];
                Chess chess = { step.first, step.second, BLACK };
                putChess(chess); //下棋
                //如果已经赢了，则直接下棋不考虑对方下棋
                if (isBlackWin()) {
                    alpha = BLACK_WIN;
                    row = step.first;
                    column = step.second;
                    //退回上一步
                    rollback(1);
                    break;
                }
                else {
                    int w;
                    Res *res = AI(depth - 1, alpha);
                    if (res == NULL)
                        w = evaluate();
                    else
                        w = res->weight;
                    rollback(1);
                    if (w > alpha) {
                        alpha = w;
                        row = step.first;
                        column = step.second;
                    }
                    if (alpha >= beta) //如果人可以获得更好的走法，则AI必然不会选择这一步走法，剪枝
                        break;
                }
            }
        }
        else
            return NULL;
    }
    Res* res = new Res(alpha, row, column);
    return res;
}

Res* ChessBoard::AI(int depth, int alpha) {
    int row, column, beta;
    beta = INF;
    if (depth == 0) {
        beta = evaluate();
        Res* res = new Res(beta, 0, 0);
        return res;
    }
    else {
        vector<Point> steps = possiblePoint();
        if (steps.size()) {
            for (int i = 0; i < steps.size(); ++i) {
                Point step = steps[i];
                Chess chess = { step.first, step.second, WHITE };
                putChess(chess); //下棋
                //如果已经赢了，则直接下棋不考虑对方下棋
                if (isWhiteWin()) {
                    beta = WHITE_WIN;
                    row = step.first;
                    column = step.second;
                    //退回上一步
                    rollback(1);
                    break;
                }
                else {
                    int w;
                    Res *res = Person(depth - 1, beta);
                    enemyAlpha = qMin(res->weight,enemyAlpha);
                    if (res == NULL)
                        w = evaluate();
                    else
                        w = res->weight;
                    rollback(1);
                    if (w < beta) {
                        beta = w;
                        row = step.first;
                        column = step.second;
                    }
                    if (beta <= alpha) //如果AI可以获得更好的走法，则人必然不会选择这一步走法，剪枝
                        break;
                }
            }
        }
        else
            return NULL;
    }

    Res* res = new Res(beta, row, column);
    return res;
}
