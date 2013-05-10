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
    callUpdateTimer.setInterval(15000);  // 15 sec
    connect(&callUpdateTimer, SIGNAL(timeout()), this, SLOT(updateCallId()));
    callUpdateTimer.start();
    lastAlertId = "";
}

void HttpsWebView::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
{    
    qDebug() << "handleSslErrors:";
    foreach (QSslError e, errors)
    {
        qDebug() << "ssl error:" << e;
    }

    reply->ignoreSslErrors(errors);
}

void HttpsWebView::handleUnsupportedContent(QNetworkReply *reply)
{
    qDebug() << "handleUnsupportedContent:";
    qDebug() << reply->header(QNetworkRequest::ContentTypeHeader);
}

void HttpsWebView::handleLoadFinished(bool finished)
{
    if ((code != "") && (code != 0))
        page()->mainFrame()->evaluateJavaScript("$('#device_code').val('"+code+"'); $('#register').click();");
}

void HttpsWebView::updateCallId()
{
    QString thisAlertId = page()->mainFrame()->evaluateJavaScript("$('div#alerts').find('div.A91Alert').attr('alert_id') | '';").toString();
    qDebug() << "AlertID:";
    qDebug() << lastAlertId;
    qDebug() << thisAlertId;
    if ((thisAlertId != "") && (thisAlertId != "0")) {
        lastAlertId = thisAlertId;
        if (lastAlertId == "")
            return;
        else if (thisAlertId != lastAlertId)
            emit alert();
    }
}
