#include "httpswebview.h++"
#include <QDebug>
#include <QNetworkReply>
#include <QSslError>

HttpsWebView::HttpsWebView(QWidget *parent) :
    QWebView(parent)
{
    connect(page()->networkAccessManager(),
                SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
                this,
                SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> & )));
}

void HttpsWebView::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
{
    qDebug() << "handleSslErrors: ";
    foreach (QSslError e, errors)
    {
        qDebug() << "ssl error: " << e;
    }

    reply->ignoreSslErrors();
}
