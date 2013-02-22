#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <QtWidgets/QWidget>

class TicTacToe : public QWidget
{
    Q_OBJECT

public:
    TicTacToe(QWidget *parent = 0);

    bool gameInProgress() const;
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    enum Kind { Empty = '-', Cross = 'X', Nought = 'O' };

    void clearBoard();
    void computerMakeMove();
    QRect cellRect(int row, int column) const;
    int cellWidth() const { return width() / 3; }
    int cellHeight() const { return height() / 3; }
    int getMove();
    void fixMistake(int lastIndex);
    QList<int> winningLocation(Kind k);
    bool threeInARow(int row1, int col1, int row3, int col3) const;

    char board[3][3];
    int weights[3][3];
    int turnNumber;

    bool computerFirst;
};

#endif
