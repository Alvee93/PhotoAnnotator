#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    //first set the flags
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
