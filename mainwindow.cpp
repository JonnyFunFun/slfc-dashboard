#include "mainwindow.h++"
#include "ui_mainwindow.h"
#include "httpswebview.h++"

#include <QDebug>
#include <QtXml/QtXml>
#include <QFile>
#include <qmath.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentlyRedrawing = false;
    // Open our config files and load some calendars
    configDocument = QDomDocument("config");
#if __APPLE__
    // Stupid OS X
    QFile configFile("../../../config.xml");
#else
    QFile configFile("config.xml");
#endif
    if (!configFile.open(QIODevice::ReadOnly))
    {
        fprintf(stderr, ("Unable to open configuration document!\n"+QCoreApplication::applicationDirPath()+"/config.xml\n").toStdString().c_str());
        return;
    }
    if (!configDocument.setContent(&configFile))
    {
        configFile.close();
        fprintf(stderr, "Unable to load configuration document!\n");
        return;
    }

    // Refresh timer
    connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh_timer()));
    refreshTimer.setInterval(3600000); // 1 hr

    QDomNodeList calList = configDocument.documentElement().elementsByTagName("calendars").at(0).toElement().elementsByTagName("feed");
    qDebug() << QString("Opening %1 calendars...").arg(calList.count());

    if (calList.count() > 0)
    {
        for (int i = 0; i < calList.count(); i++)
        {
            GoogleCalendar *cal = new GoogleCalendar(this, calList.at(i).attributes().namedItem("url").nodeValue());
            connect(cal, SIGNAL(refreshComplete()), this, SLOT(redraw()));
            connect(&refreshTimer, SIGNAL(timeout()), cal, SLOT(refreshCalendar()));
            calendars.append(cal);
        }
    }

    // m6 takehome
    calList = configDocument.documentElement().elementsByTagName("m6takehome");
    if (calList.count() > 0)
    {
        m6calendar = new GoogleCalendar(this, calList.at(0).attributes().namedItem("calendar").nodeValue());
        qDebug() << "Found M6 Takehome calendar.";
    } else {
        m6calendar = 0;
        qWarning() << "No M6 Takehome calendar found.";
    }

    // http server
    if (configDocument.documentElement().elementsByTagName("ticker").at(0).attributes().namedItem("enabled").nodeValue() == "true")
    {
        ticker_http = new TickerHttpServer(configDocument.documentElement().elementsByTagName("ticker").at(0).attributes().namedItem("httpPort").nodeValue().toInt(),
                                           configDocument.documentElement().elementsByTagName("ticker").at(0).attributes().namedItem("authToken").nodeValue(), this);
        connect(ticker_http, SIGNAL(newMessage(QString,QDateTime)), this, SLOT(handle_new_ticker_message(QString,QDateTime)));
        tickerMessages.clear();
        tickerMessages.append(QPair<QString,QDateTime>("There are no ticker messages",QDateTime::currentDateTime()));
    } else {
        tickerMessages.clear();
        tickerMessages.append(QPair<QString,QDateTime>("Ticker server disabled",QDateTime::currentDateTime()));
    }

    // active911 web view
    active911DeviceId = configDocument.documentElement().elementsByTagName("active911").at(0).attributes().namedItem("deviceId").nodeValue();

    //this->showFullScreen();
    this->setStyleSheet("background-color: black; color: white;");

    // Determine our constraints and calculate where our widgets are all going to be placed
    ui->scrollText->setGeometry(0, qCeil(height()*0.9), width(), qCeil(height()*0.1));
    ui->scrollText->setFixedHeight(qCeil(height()*0.1));

    // Our stack
    ui->stack->setGeometry(0, 0, width(), qFloor(height()*0.9));
    ui->stack->setStyleSheet("background-color: white!important; color: black!important;");

    // Page Labels
    ui->upcomingEventsLabel->setGeometry(0, 0, width(), qFloor(height()*0.08));
    ui->upcomingEventsLabel->setFont(QFont("sans-serif", height()*0.08));
    ui->m6TakeHomeLabel->setGeometry(0, 0, width(), qFloor(height()*0.08));
    ui->m6TakeHomeLabel->setFont(QFont("sans-serif", height()*0.08));

    // Upcoming events page
    ui->list_upcoming->setGeometry(0, qFloor(height()*0.08)+25, width(), height());
    ui->list_upcoming->setFont(QFont("sans-serif", height()*0.07));

    // m6 Takehome page
    ui->list_m6takehome->setGeometry(0, qFloor(height()*0.08)+25, width(), height());
    ui->list_m6takehome->setFont(QFont("sans-serif", height()*0.07));

    // Active911 web view
    ui->active911WebView->setGeometry(0, 0, width(), height());
    ui->active911WebView->load(QUrl("http://webview.active911.com/"));

    // Scrolling text
    ui->scrollText->setFont(QFont("sans-serif", height()*0.075));
    ui->scrollText->setText("Loading...");

    // progress timer for switching between screens
    connect(&progressTimer, SIGNAL(timeout()), this, SLOT(progress_timer()));
    //progressTimer.setInterval(30000); // 30 seconds
    progressTimer.setInterval(5000);

    // start the timers up
    refreshTimer.start();
    progressTimer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh_timer()
{
    // Refresh data other than the signal/slot connected data
}

void MainWindow::progress_timer()
{
    // Progress to the next screen
    int next = ui->stack->currentIndex()+1;
    if (next > ui->stack->count() - 1)
        next = 0;
    ui->stack->setCurrentIndex(next);
}

void MainWindow::handle_new_ticker_message(QString message, QDateTime expiration)
{
    tickerMessages.append(QPair<QString,QDateTime>(message, expiration));
    QList<QPair<QString, QDateTime> > goodMessages;
    for (int i = 0; i < tickerMessages.count(); ++i)
    {
        if (tickerMessages.at(i).second > QDateTime::currentDateTime())
            goodMessages.append(tickerMessages.at(i));
    }
    tickerMessages = goodMessages;
    redraw();
}

void MainWindow::redraw()
{
    // locking
    if (currentlyRedrawing)
        return;
    currentlyRedrawing = true;
    // Refresh our views
    // upcoming events
    ui->list_upcoming->clear();
    QList<CalendarEvent*> upcoming;
    for (int i = 0; i < calendars.count(); ++i)
        upcoming.append(calendars.at(i)->upcoming(10));
    qSort(upcoming.begin(), upcoming.end(), CalendarEventComparator());
    for (int i = 0; i < upcoming.count(); ++i)
        ui->list_upcoming->addItem(upcoming.at(i)->toString());
    // m6 takehome
    if (m6calendar == 0) {
        ui->list_m6takehome->clear();
        ui->list_m6takehome->addItem("There was an error loading the m6 calendar");
    } else {
        ui->list_m6takehome->clear();
        ui->list_m6takehome->addItem(" ");
        QList<CalendarEvent*> upcoming = m6calendar->upcoming(2);
        if (upcoming.count() >= 1) {
            if (upcoming.at(0)->start_date < QDateTime::currentDateTime()) {
                // Currently signed out
                ui->list_m6takehome->addItem(QString("Current EMT: %1").arg(upcoming.at(0)->title));
                ui->list_m6takehome->addItem(QString("Until %1").arg(upcoming.at(0)->end_date.toString("hh:mm")));
                ui->list_m6takehome->addItem(" ");
                if (upcoming.count() == 2) {
                    ui->list_m6takehome->addItem(QString("Next: %1").arg(upcoming.at(1)->title));
                    ui->list_m6takehome->addItem(QString("On %1 at %2").arg(upcoming.at(1)->start_date.toString("ddd MMM dd")).arg(upcoming.at(1)->start_date.toString("hh:mm")));
                } else {
                    ui->list_m6takehome->addItem("There are no upcoming shifts scheduled.");
                }
            } else {
                ui->list_m6takehome->addItem("No EMT currently assigned.");
                ui->list_m6takehome->addItem(" ");
                if (upcoming.count() == 1) {
                    ui->list_m6takehome->addItem(QString("Next: %1").arg(upcoming.at(0)->title));
                    ui->list_m6takehome->addItem(QString("On %1 at %2").arg(upcoming.at(0)->start_date.toString("ddd MMM dd")).arg(upcoming.at(0)->start_date.toString("hh:mm")));
                } else {
                    ui->list_m6takehome->addItem("There are no upcoming shifts scheduled.");
                }
            }
        } else {
            ui->list_m6takehome->addItem(" ");
            ui->list_m6takehome->addItem("There are no scheduled shifts.");
        }
    }

    // Ticker message
    QString totalMessage;
    for (int i = 0; i < tickerMessages.count(); ++i)
        totalMessage += tickerMessages.at(i).first + "  :  ";
    totalMessage.chop(5);
    ui->scrollText->setText(totalMessage);

    // Center stuff
    for (int i = 0; i < ui->list_m6takehome->count(); ++i)
        ui->list_m6takehome->item(i)->setTextAlignment(Qt::AlignCenter);

    currentlyRedrawing = false;
}
