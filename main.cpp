#include <QApplication>

//local
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("ELNA");
    QCoreApplication::setOrganizationDomain("elnavf.ru");
    QCoreApplication::setApplicationName("BSIO Manager");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
