#include <QtGui/QApplication>
#include <QNetworkProxy>
#include "mainwindow.h++"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // set proxy information for network connectivity
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::NoProxy);
    QNetworkProxy::setApplicationProxy(proxy);

    // make it happen!
    MainWindow w;
    w.show();

    // main loop
    return a.exec();
}
