#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    a.setApplicationDisplayName("picsure");
    a.setApplicationName("picsure");
    a.setApplicationVersion("0.1.0");
    a.setOrganizationName("adasoft");
    w.show();
    return a.exec();
}
