#ifndef HTTPSWEBVIEW_H
#define HTTPSWEBVIEW_H

#include <QWebView>

class HttpsWebView : public QWebView
{
    Q_OBJECT
public:
    HttpsWebView(QWidget *parent = 0);
private slots:
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);
    void navigationFinished(QNetworkReply* reply);
    void statusBarMessageChange(QString message);
    void foundUnsupportedContent(QNetworkReply* reply);
};

#endif // HTTPSWEBVIEW_H
