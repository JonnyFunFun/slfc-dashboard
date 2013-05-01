#include "calendarevent.h++"
#include "rfc3339.h++"

CalendarEvent::CalendarEvent(QString event_title, QDateTime start, QDateTime end, QString cont)
{
    title = event_title;
    start_date = start;
    end_date = end;
    content = cont;
    qDebug() << QString("Added %1 from %2 to %3").arg(title).arg(start_date.toString("dd.MM.yyyy hh:mm")).arg(end_date.toString("dd.MM.yyyy hh:mm")).toStdString().c_str();
}

QString CalendarEvent::toString()
{
    if ((content != "") && (content != 0))
        return QString("%1 - %2\n\t%3").arg(start_date.toString("MMM dd hh:mm"), title, content);
    else
        return QString("%1 - %2").arg(start_date.toString("MMM dd hh:mm"), title);
}

CalendarEventComparator::CalendarEventComparator() {}
bool CalendarEventComparator::operator ()(const CalendarEvent *left, const CalendarEvent *right) const
{
    return left->start_date < right->start_date;
}
