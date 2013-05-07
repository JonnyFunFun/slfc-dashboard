#include <QtGui/QApplication>
#include <QNetworkProxy>
#include <QWebSettings>
#include <QSslConfiguration>
#include <QSsl>
#include <QSslCipher>
#include "mainwindow.h++"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // set proxy information for network connectivity
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::NoProxy);
    QNetworkProxy::setApplicationProxy(proxy);

    QWebSettings *settings = QWebSettings::globalSettings();
    settings->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebSettings::JavaEnabled, true);
    settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    settings->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    settings->setAttribute(QWebSettings::WebGLEnabled, true);
    settings->setAttribute(QWebSettings::LocalStorageEnabled, true);

#ifndef QT_NO_OPENSSL
    QSslConfiguration sslCfg = QSslConfiguration::defaultConfiguration();
    QList<QSslCertificate> ca_list = sslCfg.caCertificates();
#if __APPLE__
    // Stupid OS X
    QList<QSslCertificate> ca_new = QSslCertificate::fromData("/System/Library/OpenSSL/certs");
#else
    QList<QSslCertificate> ca_new = QSslCertificate::fromData("/usr/lib/ssl/certs");
#endif
    ca_list += ca_new;
    sslCfg.setCaCertificates(ca_list);
    sslCfg.setProtocol(QSsl::AnyProtocol);
    sslCfg.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslCfg.setCiphers(QSslSocket::supportedCiphers());
    QSslConfiguration::setDefaultConfiguration(sslCfg);
#endif

    // make it happen!
    MainWindow w;
    w.show();

    // main loop
    return a.exec();
}
