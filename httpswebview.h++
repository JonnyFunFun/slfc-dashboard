#ifndef HTTPSWEBVIEW_H
#define HTTPSWEBVIEW_H

#include <QWebView>

class HttpsWebView : public QWebView
{
    Q_OBJECT
public:
    HttpsWebView(QWidget *parent = 0);
    QString code;
private slots:
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);
    void handleUnsupportedContent(QNetworkReply* reply);
    void handleLoadFinished(bool finished);
};

#endif // HTTPSWEBVIEW_H
