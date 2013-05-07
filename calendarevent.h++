#ifndef CALENDAREVENT_H
#define CALENDAREVENT_H

#include <QDateTime>
#include <QtXml/QtXml>

class CalendarEvent
{
public:
    friend class CalendarEventComparator;
    CalendarEvent(QString event_title, QDateTime start, QDateTime end, QString cont);
    QString title;
    QString content;
    QDateTime start_date;
    QDateTime end_date;
    QString toString();
    QString drillFormat();
    bool isSpecialDrill();
};

class CalendarEventComparator
{
public:
    CalendarEventComparator();
    bool operator()(const CalendarEvent *left, const CalendarEvent *right) const;
};

#endif // CALENDAREVENT_H
