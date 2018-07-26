#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
//    QTextCodec* defaultTextCodec = QTextCodec::codecForName("Windows-1251");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
