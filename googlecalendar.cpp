#include "googlecalendar.h++"
#include <QDebug>
#include <QtXml/QtXml>
#include <QEventLoop>

GoogleCalendar::GoogleCalendar(QObject *parent, QString url) :
    QObject(parent)
{
    QUrl parsed(url);
    access_manager = new QNetworkAccessManager(this);
    connect(access_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(dataDownloadComplete(QNetworkReply*)));
    connect(access_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(dataDownloadSSLError(QNetworkReply*,QList<QSslError>)));
    webpath = parsed;
    qDebug() << QString("Added calendar for %1").arg(url);
    refreshCalendar();
}

QList<CalendarEvent*> GoogleCalendar::upcoming(int count)
{
    qSort(events.begin(), events.end(), CalendarEventComparator());
    QList<CalendarEvent*> ret;
    if (count > events.count())
        count = events.count();
    for (int i = 0; i < count; ++i)
        ret.append(events.at(i));
    return ret;
}

void GoogleCalendar::refreshCalendar()
{
    access_manager->get(QNetworkRequest(webpath));
}

void GoogleCalendar::dataDownloadSSLError(QNetworkReply *rep, QList<QSslError> errors)
{
    fprintf(stderr, "Unable to open gcal XML, SSL errors:\n");
    for (int i = 0; i < errors.count(); ++i)
        fprintf(stderr, (errors.at(i).errorString()+"\n").toStdString().c_str());
}

void GoogleCalendar::dataDownloadComplete(QNetworkReply *rep)
{
    if ((rep->error() != 0) && (rep->error() != QNetworkReply::UnknownContentError))
    {
        fprintf(stderr, QString("HTTP error %1\n").arg(rep->error()).toStdString().c_str());
        return;
    }
    QString content = rep->readAll();
    QDomDocument *xml = new QDomDocument("feed");
    if (!xml->setContent(content))
    {
        fprintf(stderr, ("Unable to open gcal XML from:\n"+webpath.toString()+"\n").toStdString().c_str());
        qDebug() << content;
        return;
    }
    events.clear();
    qDebug() << QString("Loading %1 entries").arg(xml->documentElement().elementsByTagName("entry").count());
    for (int i = 0; i < xml->documentElement().elementsByTagName("entry").count(); ++i)
        events.append(new CalendarEvent(&xml->documentElement().elementsByTagName("entry").at(i)));
    qSort(events.begin(), events.end(), CalendarEventComparator());
    purgePast();
    emit refreshComplete();
    delete xml;
}

void GoogleCalendar::purgePast()
{
    QList<CalendarEvent*> goodEvents;
    for (int i = 0; i < events.count(); ++i)
    {
        if (events.at(i)->end_date > QDateTime::currentDateTime())
            goodEvents.append(events.at(i));
    }
    events = goodEvents;
}
