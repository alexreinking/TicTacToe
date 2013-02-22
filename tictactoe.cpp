#include <QtWidgets/QtWidgets>
#include <cstdlib>
#include <ctime>
#include "tictactoe.h"

TicTacToe::TicTacToe(QWidget *parent)
    : QWidget(parent)
{
    srand((unsigned)time(0));
    computerFirst = true;
    clearBoard();
    setWindowTitle(tr("Tic-Tac-Toe"));
}

QSize TicTacToe::sizeHint() const
{
    return QSize(200, 200);
}

void TicTacToe::clearBoard()
{
    for (int row = 0; row < 3; ++row) {
        for (int column = 0; column < 3; ++column) {
            board[row][column] = Empty;
        }
    }
    turnNumber = 0;
    computerFirst = !computerFirst;
    if(computerFirst)
        computerMakeMove();
}

bool TicTacToe::gameInProgress() const
{
    return turnNumber > 0 && turnNumber < 9;
}

void TicTacToe::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(Qt::red, 1));
    painter.drawLine(cellWidth(), 0, cellWidth(), height());
    painter.drawLine(2 * cellWidth(), 0, 2 * cellWidth(), height());
    painter.drawLine(0, cellHeight(), width(), cellHeight());
    painter.drawLine(0, 2 * cellHeight(), width(), 2 * cellHeight());

    painter.setPen(QPen(Qt::blue, 2));
    for (int row = 0; row < 3; ++row) {
        for (int column = 0; column < 3; ++column) {
            QRect r = cellRect(row, column);
            if (board[row][column] == Cross) {
                painter.drawLine(r.topLeft(), r.bottomRight());
                painter.drawLine(r.topRight(), r.bottomLeft());
            } else if (board[row][column] == Nought) {
                painter.drawEllipse(r);
            }
        }
    }

    painter.setPen(QPen(Qt::green, 3));
    for (int row = 0; row < 3; ++row) {
        if (threeInARow(row, 0, row, 2)) {
            int y = cellRect(row, 0).center().y();
            painter.drawLine(0, y, width(), y);
            turnNumber = 9;
        }
    }
    for (int column = 0; column < 3; ++column) {
        if (threeInARow(0, column, 2, column)) {
            int x = cellRect(0, column).center().x();
            painter.drawLine(x, 0, x, height());
            turnNumber = 9;
        }
    }
    if (threeInARow(0, 0, 2, 2)) {
        painter.drawLine(0, 0, width(), height());
        turnNumber = 9;
    }
    if (threeInARow(0, 2, 2, 0)) {
        painter.drawLine(0, height(), width(), 0);
        turnNumber = 9;
    }
}

void TicTacToe::mousePressEvent(QMouseEvent *event)
{
    if (turnNumber == 9) {
        clearBoard();
        update();
    } else {
        for (int row = 0; row < 3; ++row) {
            for (int column = 0; column < 3; ++column) {
                QRect r = cellRect(row, column);
                if (r.contains(event->pos())) {
                    if (board[row][column] == Empty) {
                        board[row][column] = ((!computerFirst) ? Cross : Nought);
                        ++turnNumber;
                        repaint();
                    }
                    break;
                }
            }
        }
        computerMakeMove();
    }
}

void TicTacToe::computerMakeMove()
{
    if(turnNumber == 9)
        return;
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            weights[i][j] = 0;
    weights[0][0] = 1;
    weights[0][2] = 1;
    if(turnNumber != 0)
        weights[1][1] = 1;
    weights[2][0] = 1;
    weights[2][2] = 1;
    for(int row = 0; row < 3; row++)
    {
        QString foo;
        foo += board[row][0];
        foo += board[row][1];
        foo += board[row][2];
        if(!(foo.indexOf('X') != -1 && foo.indexOf('O') != -1))
        {
            weights[row][0]++;
            weights[row][1]++;
            weights[row][2]++;
        }
    }
    for(int col = 0; col < 3; col++)
    {
        QString foo;
        foo += board[0][col];
        foo += board[1][col];
        foo += board[2][col];
        if(!(foo.indexOf('X') != -1 && foo.indexOf('O') != -1))
        {
            weights[0][col]++;
            weights[1][col]++;
            weights[2][col]++;
        }
    }
    QString foo;
    foo += board[0][0];
    foo += board[1][1];
    foo += board[2][2];
    if(!(foo.indexOf('X') != -1 && foo.indexOf('O') != -1))
    {
        weights[0][0]++;
        weights[1][1]++;
        weights[2][2]++;
    }
    foo = "";
    foo += board[0][2];
    foo += board[1][1];
    foo += board[2][0];
    if(!(foo.indexOf('X') != -1 && foo.indexOf('O') != -1))
    {
        weights[0][2]++;
        weights[1][1]++;
        weights[2][0]++;
    }
    for(int row = 0; row < 3; row++) {
        for(int col = 0; col < 3; col++) {
            if(board[row][col] != '-')
                weights[row][col] = 0;
        }
    }
    int pos = getMove();
    if (*(board[0] + pos) == Empty) {
        *(board[0] + pos) = ((computerFirst) ? Cross : Nought);
        fixMistake(pos);
        ++turnNumber;
        update();
    }
}

int TicTacToe::getMove()
{
    QList<int> indexes;
    int highest = 0;
    for(int i = 0; i < 9; i++)
    {
        if(*(weights[0]+i) > highest)
        {
            indexes.clear();
            indexes.append(i);
            highest = *(weights[0]+i);
        } else if(*(weights[0]+i) == highest) {
            indexes.append(i);
        }
    }
    if(indexes.empty())
    {
        for(int i = 0; i < 9; i++)
        {
            if(*(board[0]+i) == '-')
                return i;
        }
    }
    return indexes.at(rand() % (indexes.size()));
}

QList<int> TicTacToe::winningLocation(Kind k)
{
    QList<int> vals;
    for(int row = 0; row < 3; row++) {
        if(board[row][0] == Empty && (board[row][1] == board[row][2]) && (board[row][1] == k))
            vals.append((row*3));
        if(board[row][1] == Empty && (board[row][0] == board[row][2]) && (board[row][0] == k))
            vals.append((row*3)+1);
        if(board[row][2] == Empty && (board[row][1] == board[row][0]) && (board[row][1] == k))
            vals.append((row*3)+2);
    }
    for(int col = 0; col < 3; col++) {
        if(board[0][col] == Empty && (board[1][col] == board[2][col]) && (board[1][col] == k))
            vals.append(col);
        if(board[1][col] == Empty && (board[2][col] == board[0][col]) && (board[2][col] == k))
            vals.append(3+col);
        if(board[2][col] == Empty && (board[1][col] == board[0][col]) && (board[1][col] == k))
            vals.append(6+col);
    }
    if(board[1][1] == k)
    {
        if(board[0][0] == k && board[2][2] == Empty)
            vals.append(8);
        if(board[0][2] == k && board[2][0] == Empty)
            vals.append(6);
        if(board[2][0] == k && board[0][2] == Empty)
            vals.append(2);
        if(board[2][2] == k && board[0][0] == Empty)
            vals.append(0);
    }
    if(board[0][0] == board[2][2] && board[0][0] == k && board[1][1] == Empty)
        vals.append(4);
    if(board[0][2] == board[2][0] && board[0][2] == k && board[1][1] == Empty)
        vals.append(4);
    return vals;
}

void TicTacToe::fixMistake(int lastMove)
{
    Kind me = ((computerFirst) ? Cross : Nought);
    Kind them = ((!computerFirst) ? Cross : Nought);
    *(board[0] + lastMove) = Empty;
    if(!winningLocation(me).isEmpty()) {
        int good = winningLocation(me).first();
        *(board[0] + good) = me;
        return;
    }
    if(!winningLocation(them).isEmpty()) {
        int good = winningLocation(them).first();
        *(board[0] + good) = me;
        return;
    }
    *(board[0] + lastMove) = me;
    for(int i = 0; i < 9; i++)
    {
        char foo = *(board[0]+i);
        if(*(board[0]+i) == Empty)
        {
            *(board[0]+i) = them;
            int cat = winningLocation(them).size();
            if(winningLocation(them).size() > 1 && (winningLocation(me).size() == 0)) { //issue
                *(board[0]+lastMove) = Empty;
                *(board[0]+i) = Empty;
                for(int j = 0; j < 9; j++)
                {
                    if(*(board[0]+j) == Empty)
                    {
                        *(board[0]+j) = me;
                        if(winningLocation(me).size() >= 1 && winningLocation(them).size() == 0) {
                            int tmp = winningLocation(me).first();
                            *(board[0]+tmp) = them;
                            if(winningLocation(them).size() <= 1) {
                                *(board[0]+tmp) = Empty;
                                return;
                            }
                            *(board[0]+tmp) = Empty;
                        }
                        *(board[0]+j) = Empty;
                    }
                }
            }
            *(board[0]+i) = Empty;
        }
    }
    QList<QPair<int, int> > goodMoves;
    for(int i = 0; i < 9; i++)
    {
        *(board[0]+lastMove) = Empty;
        if(*(board[0]+i) == Empty)
        {
            *(board[0]+i) = me;
            if(winningLocation(me).size() && winningLocation(them).size() == 0)
            {
                int m = winningLocation(me).first();
                *(board[0]+m) = them;
                if(winningLocation(them).size() != 2)
                    goodMoves.append(QPair<int, int>(winningLocation(me).size(), i));
                *(board[0]+m) = Empty;
            }
            *(board[0]+i) = Empty;
        }
        *(board[0]+lastMove) = me;
    }
    qSort(goodMoves);
    if(goodMoves.size()) {
        *(board[0]+lastMove) = Empty;
        *(board[0]+goodMoves.last().second) = me;
    }
}

QRect TicTacToe::cellRect(int row, int column) const
{
    const int HMargin = width() / 30;
    const int VMargin = height() / 30;
    return QRect(column * cellWidth() + HMargin,
                 row * cellHeight() + VMargin,
                 cellWidth() - 2 * HMargin,
                 cellHeight() - 2 * VMargin);
}

bool TicTacToe::threeInARow(int row1, int column1, int row3,
                            int column3) const
{
    int row2 = (row1 + row3) / 2;
    int column2 = (column1 + column3) / 2;

    return board[row1][column1] != Empty
            && board[row2][column2] == board[row1][column1]
            && board[row3][column3] == board[row1][column1];
}
