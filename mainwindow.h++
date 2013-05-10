#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml/QtXml>
#include <QTimer>
#include "scrolltext.h++"
#include "googlecalendar.h++"
#include "tickerhttpserver.h++"
#include <QEventLoop>
#include <QPair>
#include <stdint.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadPersistence();
public slots:
    virtual void redraw();
    virtual void persistData();
    virtual void handleAlert();
private:
    Ui::MainWindow *ui;
    bool currentlyRedrawing;
    QTimer refreshTimer;
    QTimer progressTimer;
    QTimer redrawTimer;
    QTimer alertProceedTimer;
    QDomDocument configDocument;
    QList<GoogleCalendar*> calendars;
    QList<QPair<QString,QDateTime> > tickerMessages;
    GoogleCalendar *m6calendar;
    GoogleCalendar *drillCalendar;
    QEventLoop evloop;
    TickerHttpServer *ticker_http;
    QString active911DeviceId;
private slots:
    virtual void handle_new_ticker_message(QString message, QDateTime expiration);
    virtual void refresh_timer();
    virtual void progress_timer();
    virtual void clearAlertProceed();
};

#endif // MAINWINDOW_H
