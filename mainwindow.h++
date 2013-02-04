#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml/QtXml>
#include <QTimer>
#include "scrolltext.h++"
#include "googlecalendar.h++"
#include "tickerhttpserver.h++"
#include <QEventLoop>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    virtual void redraw();

private:
    Ui::MainWindow *ui;
    QTimer refreshTimer;
    QTimer progressTimer;
    QDomDocument configDocument;
    QList<GoogleCalendar*> calendars;
    GoogleCalendar *m6calendar;
    QEventLoop evloop;
    TickerHttpServer *ticker_http;
    QString active911DeviceId;
private slots:
    virtual void refresh_timer();
    virtual void progress_timer();
};

#endif // MAINWINDOW_H
