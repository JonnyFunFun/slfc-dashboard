#include "tickerhttpserver.h++"
#include <QRegExp>
#include <QStringList>
#include <QDebug>
#include <qhttpconnection.h>
#include "rfc3339.h++"

TickerHttpServer::TickerHttpServer(quint64 port, QString token, QObject *parent) :
    QObject(parent)
{
    server = new QHttpServer;
    server->listen(port);
    authToken = token;
    qDebug() << QString("Ticker HTTP Server listening 0.0.0.0:%1").arg(port);
    connect(server, SIGNAL(newRequest(QHttpRequest*,QHttpResponse*)), this, SLOT(handle(QHttpRequest*,QHttpResponse*)));
}

void TickerHttpServer::handle(QHttpRequest *req, QHttpResponse *resp)
{
    resp->setHeader("Content-Type", "application/json");
    if (req->method() != QHttpRequest::HTTP_POST) {
        resp->writeHead(400);
        resp->write(tr("{'Error': 400, 'Message': 'Bad request'}"));
        resp->end();
        resp->disconnect();
        qDebug() << QString("Bad request method from %1 (%2)").arg(req->remoteAddress(), req->method());
        return;
    }
    if (req->header("x-auth") != authToken) {
        resp->writeHead(401);
        resp->write(tr("{'Error': 401, 'Message': 'Not authorized or invalid authorization token'}"));
        resp->end();
        resp->disconnect();
        qDebug() << QString("Bad authentication token from %1 (%2)").arg(req->remoteAddress(), req->header("x-auth"));
        return;
    }

    connect(req, SIGNAL(data(QByteArray)), this, SLOT(processRequest(QByteArray)));
}

void TickerHttpServer::processRequest(QByteArray data) {
    // date format: yyyy-MM-ddThh:mm:ss.zzzZ
    QString body = QString(data);
    QString responseHead;
    QString responseBody;
    QHttpRequest *req = qobject_cast<QHttpRequest*>(QObject::sender());
    QRegExp messageFormat("^(.*)=>(\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}\\.\\d{3}Z)$");
    if (messageFormat.indexIn(body) > -1) {
        responseHead = "200 OK";
        responseBody = "{'Status': 'ok'}";
        qDebug() << QString("Received ticker message %1").arg(messageFormat.cap(1));
        emit newMessage(messageFormat.cap(1), RFC3339::fromString(messageFormat.cap(2)));
    } else {
        responseHead = "406 Not Acceptable";
        responseBody = "{'Error': 406, 'Message': 'Message malformed or not acceptable'}";
        qDebug() << "Received bad ticker message format";
    }
    req->connection()->write(QString("HTTP/1.1 %1 \nContent-Type: application/json\nConnection: close\nDate: %2\n\n").arg(responseHead, QDateTime::currentDateTime().toString("ddd, dd MMM yyyy hh:mm:ss EST")).toAscii());
    req->connection()->write(responseBody.toAscii());
    req->connection()->end();
}


