#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.setStyleSheet("background-color: black;");
    w.setWindowTitle("Server");

    w.show();
    return a.exec();
}
