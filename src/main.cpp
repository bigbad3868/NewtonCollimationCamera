#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("NewtonCollimationCamera");
    QCoreApplication::setOrganizationDomain("newtoncollimationcamera.org");
    QCoreApplication::setApplicationName("Newton Collimation Camera");

    MainWindow w;
    w.show();
    return a.exec();
}
