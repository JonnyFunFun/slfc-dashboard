#ifndef TICKERHTTPSERVER_H
#define TICKERHTTPSERVER_H

#include <QObject>
#include <QDateTime>

#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"

class TickerHttpServer : public QObject
{
    Q_OBJECT
public:
    explicit TickerHttpServer(quint64 port, QString token, QObject *parent = 0);
private:
    QString authToken;
    QHttpServer *server;
signals:
    void newMessage(QString msg, QDateTime expiration);
public slots:
    
private slots:
    void processRequest(QByteArray data);
    void handle(QHttpRequest *req, QHttpResponse *resp);
};

#endif // TICKERHTTPSERVER_H
