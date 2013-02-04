#include "tickerhttpserver.h++"
#include <QRegExp>
#include <QStringList>
#include <QDebug>

TickerHttpServer::TickerHttpServer(quint64 port, QObject *parent) :
    QObject(parent)
{
    server = new QHttpServer;
    server->listen(port);
    qDebug() << QString("Ticker HTTP Server listening 0.0.0.0:%1").arg(port);
    connect(server, SIGNAL(newRequest(QHttpRequest*,QHttpResponse*)), this, SLOT(handle(QHttpRequest*,QHttpResponse*)));
}

void TickerHttpServer::handle(QHttpRequest *req, QHttpResponse *resp)
{
    //resp->setHeader("Content-Type", "text/html");
    resp->writeHead(200);
    QString reply = tr("<html><head><title>Greeting App</title></head><body><h1>Hello!</h1></body></html>");
    resp->write(reply.toAscii());
    resp->end();
    resp->disconnect();
}


