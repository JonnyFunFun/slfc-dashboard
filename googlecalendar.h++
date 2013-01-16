#ifndef GOOGLECALENDAR_H
#define GOOGLECALENDAR_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QUrl>
#include <QNetworkReply>
#include <QSslError>
#include <QNetworkAccessManager>
#include "calendarevent.h++"

class GoogleCalendar : public QObject
{
    Q_OBJECT
public:
    explicit GoogleCalendar(QObject *parent = 0, QString webpath = 0);
    void refresh();
    QList<CalendarEvent*> upcoming(int count = 3);
    void purgePast();
signals:
    void refreshComplete();
public slots:
    void refreshCalendar();
private:
    QUrl webpath;
    QNetworkAccessManager *access_manager;
    QList<CalendarEvent*> events;
private slots:
    void dataDownloadComplete(QNetworkReply *rep);
    void dataDownloadSSLError(QNetworkReply *rep, QList<QSslError> errors);
};

#endif // GOOGLECALENDAR_H
