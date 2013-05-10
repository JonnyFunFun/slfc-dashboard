#ifndef HTTPSWEBVIEW_H
#define HTTPSWEBVIEW_H

#include <QWebView>
#include <QTimer>

class HttpsWebView : public QWebView
{
    Q_OBJECT
public:
    HttpsWebView(QWidget *parent = 0);
    QString code;
private:
    QString lastAlertId;
    QTimer callUpdateTimer;
    void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID);
signals:
    void alert();
private slots:
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);
    void handleUnsupportedContent(QNetworkReply* reply);
    void handleLoadFinished(bool finished);
    void updateCallId();
};

#endif // HTTPSWEBVIEW_H
