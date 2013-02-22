#include <QApplication>
#include "tictactoe.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TicTacToe toe;
    toe.show();
    return app.exec();
}
