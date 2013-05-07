#include "httpswebview.h++"
#include <QDebug>
#include <QNetworkReply>
#include <QSslError>
#include <QWebFrame>
#include <QWebElement>

HttpsWebView::HttpsWebView(QWidget *parent) :
    QWebView(parent)
{
    connect(page()->networkAccessManager(),
                SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
                this,
                SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> & )));
    connect(page(),
            SIGNAL(unsupportedContent(QNetworkReply*)),
            this, SLOT(handleUnsupportedContent(QNetworkReply*)));
    connect(page(), SIGNAL(loadFinished(bool)), this, SLOT(handleLoadFinished(bool)));
}

void HttpsWebView::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
{    
    qDebug() << "handleSslErrors: ";
    foreach (QSslError e, errors)
    {
        qDebug() << "ssl error: " << e;
    }

    reply->ignoreSslErrors(errors);
}

void HttpsWebView::handleUnsupportedContent(QNetworkReply *reply)
{
    qDebug() << "handleUnsupportedContent: ";
    qDebug() << reply->header(QNetworkRequest::ContentTypeHeader);
}

void HttpsWebView::handleLoadFinished(bool finished)
{
    qDebug() << this->url().toString();
}
