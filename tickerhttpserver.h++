#ifndef TICKERHTTPSERVER_H
#define TICKERHTTPSERVER_H

#include <QObject>

#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"

class TickerHttpServer : public QObject
{
    Q_OBJECT
public:
    explicit TickerHttpServer(quint64 port, QObject *parent = 0);
private:
    QHttpServer *server;
signals:
    
public slots:
    
private slots:
    void handle(QHttpRequest *req, QHttpResponse *resp);
};

#endif // TICKERHTTPSERVER_H
