#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RInside R(argc, argv);
    MainWindow w(0,R);
    w.show();

    return a.exec();
}
