#include "rfc3339.h++"
#include <QDateTime>
#include <QLocale>
#include <QDebug>

QDateTime RFC3339::fromString(const QString &dt) {
    if (dt.length() == 10) {
        return QDateTime::fromString(dt, "yyyy-MM-dd");
    } else {
        // left(23) strips out the superfluous timezone information
        return QDateTime::fromString(dt.left(23), "yyyy-MM-ddThh:mm:ss.zzz");
    }
}

QString RFC3339::toString(const QDateTime & dt) {
    return dt.toString("yyyy-MM-ddThh:mm:ss.zzzZ");
}
