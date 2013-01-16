#include "calendarevent.h++"
#include "rfc3339.h++"

CalendarEvent::CalendarEvent(QDomNode *xmlNode)
{
    title = xmlNode->firstChildElement("title").text();
    start_date = RFC3339::fromString(xmlNode->firstChildElement("gd:when").attributes().namedItem("startTime").nodeValue());
    end_date = RFC3339::fromString(xmlNode->firstChildElement("gd:when").attributes().namedItem("endTime").nodeValue());
    qDebug() << QString("Added %1 from %2 to %3").arg(title).arg(start_date.toString("dd.MM.yyyy hh:mm")).arg(end_date.toString("dd.MM.yyyy hh:mm")).toStdString().c_str();
}

QString CalendarEvent::toString()
{
    return QString("%1 - %2").arg(start_date.toString("ddd MMM dd hh:mm"), title);
}

CalendarEventComparator::CalendarEventComparator() {}
bool CalendarEventComparator::operator ()(const CalendarEvent *left, const CalendarEvent *right) const
{
    return left->start_date < right->start_date;
}
