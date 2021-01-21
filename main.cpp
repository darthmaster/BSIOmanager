#include <QApplication>

//local
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("ELNA");
    QApplication::setOrganizationDomain("elnavf.ru");
    QApplication::setApplicationName("BSIO Manager");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
