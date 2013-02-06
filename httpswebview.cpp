#include "httpswebview.h++"
#include <QDebug>
#include <QNetworkReply>
#include <QSslError>
#include <QWebSettings>
#include <QWebFrame>
#include <QWebElement>

HttpsWebView::HttpsWebView(QWidget *parent) :
    QWebView(parent)
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    connect(page()->networkAccessManager(),
                SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )),
                this,
                SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> & )));
    connect(page()->networkAccessManager(),
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(navigationFinished(QNetworkReply*)));
    connect(page(),
            SIGNAL(statusBarMessage(QString)),
            this,
            SLOT(statusBarMessageChange(QString)));
    connect(page(),
            SIGNAL(unsupportedContent(QNetworkReply*)),
            this,
            SLOT(foundUnsupportedContent(QNetworkReply*)));

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

void HttpsWebView::navigationFinished(QNetworkReply* reply)
{
    qDebug() << "HttpsWebView Navigation Finished";
    qDebug() << this->page()->mainFrame()->baseUrl();
    qDebug() << this->page()->mainFrame()->toHtml();
}

void HttpsWebView::statusBarMessageChange(QString message)
{
    qDebug() << message;
}

void HttpsWebView::foundUnsupportedContent(QNetworkReply *reply)
{
    qDebug() << "HttpsWebView Unsupported Content";
}
