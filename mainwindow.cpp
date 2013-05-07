#include "mainwindow.h++"
#include "ui_mainwindow.h"
#include "httpswebview.h++"

#include <QDebug>
#include <QtXml/QtXml>
#include <QFile>
#include <qmath.h>
#include <QSslSocket>
#include <QColor>

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


    // Load any saved data
    loadPersistence();

    bool ok = true;
    uint32_t progressCycle = configDocument.documentElement().attributes().namedItem("cycleTime").nodeValue().toUInt(&ok);
    if (!ok)
        progressCycle = 30;
    uint32_t refreshCycle = configDocument.documentElement().attributes().namedItem("refreshTime").nodeValue().toUInt(&ok);
    if (!ok)
        refreshCycle = 30;
    uint32_t redrawCycle = configDocument.documentElement().attributes().namedItem("redrawTime").nodeValue().toUInt(&ok);
    if (!ok)
        redrawCycle = 10;

    // Refresh timer
    connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh_timer()));
    refreshTimer.setInterval(refreshCycle * 60000);

    // Redraw timer
    connect(&redrawTimer, SIGNAL(timeout()), this, SLOT(redraw()));
    redrawTimer.setInterval(redrawCycle * 60000);

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
    if ((calList.count() > 0) && (calList.at(0).attributes().namedItem("enabled").nodeValue().toLower() == "true"))
    {
        m6calendar = new GoogleCalendar(this, calList.at(0).attributes().namedItem("calendar").nodeValue());
        qDebug() << "Found M6 Takehome calendar.";
    } else {
        m6calendar = 0;
        qWarning() << "No M6 Takehome calendar found.";
    }

    // drills
    calList = configDocument.documentElement().elementsByTagName("drills");
    if ((calList.count() > 0) && (calList.at(0).attributes().namedItem("enabled").nodeValue().toLower() == "true"))
    {
        drillCalendar = new GoogleCalendar(this, calList.at(0).attributes().namedItem("calendar").nodeValue());
        qDebug() << "Found drill calendar.";
    } else {
        drillCalendar = 0;
        qWarning() << "No drill calendar found";
    }

    // http server
    if (configDocument.documentElement().elementsByTagName("ticker").at(0).attributes().namedItem("enabled").nodeValue() == "true")
    {
        ticker_http = new TickerHttpServer(configDocument.documentElement().elementsByTagName("ticker").at(0).attributes().namedItem("httpPort").nodeValue().toInt(),
                                           configDocument.documentElement().elementsByTagName("ticker").at(0).attributes().namedItem("authToken").nodeValue(), this);
        connect(ticker_http, SIGNAL(newMessage(QString,QDateTime)), this, SLOT(handle_new_ticker_message(QString,QDateTime)));
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
    ui->scrollText->setFixedHeight(qCeil(height()*0.09));

    // Our stack
    ui->stack->setGeometry(0, 0, width(), qFloor(height()*0.9));
    ui->stack->setStyleSheet("background-color: white!important; color: black!important;");

    // Page Labels
    ui->upcomingEventsLabel->setGeometry(0, 0, width(), qFloor(height()*0.09));
    ui->upcomingEventsLabel->setFont(QFont("Droid Sans Mono", height()*0.075));
    ui->m6TakeHomeLabel->setGeometry(0, 0, width(), qFloor(height()*0.09));
    ui->m6TakeHomeLabel->setFont(QFont("Droid Sans Mono", height()*0.075));
    ui->upcomingDrillsLabel->setGeometry(0, 0, width(), qFloor(height()*0.09));
    ui->upcomingDrillsLabel->setFont(QFont("Droid Sans Mono", height()*0.075));

    // Upcoming events page
    ui->list_upcoming->setGeometry(0, qFloor(height()*0.08)+25, width(), height());
    ui->list_upcoming->setFont(QFont("Droid Sans Mono", height()*0.04));

    // m6 Takehome page
    ui->list_m6takehome->setGeometry(0, qFloor(height()*0.08)+25, width(), height());
    ui->list_m6takehome->setFont(QFont("Droid Sans Mono", height()*0.04));

    // upcoming drills
    ui->list_drills->setGeometry(0, qFloor(height()*0.08)+25, width(), height());
    ui->list_drills->setFont(QFont("Droid Sans Mono", height()*0.06));

    // Active911 web view
    ui->active911WebView->setGeometry(0, 0, width(), height());

    // Scrolling text
    ui->scrollText->setFont(QFont("Droid Sans Mono", height()*0.07));
    ui->scrollText->setText("Loading...");

    // progress timer for switching between screens
    connect(&progressTimer, SIGNAL(timeout()), this, SLOT(progress_timer()));
    progressTimer.setInterval(progressCycle * 1000);

    if (!QSslSocket::supportsSsl()) {
        //delete ui->active911page;
        qWarning() << "Device does not support SSL - disabling Active911";
    } else if (configDocument.documentElement().elementsByTagName("active911").at(0).attributes().namedItem("enabled").nodeValue().toLower() == "false") {
        delete ui->active911Page;
    } else {
        ui->active911WebView->setUrl(QUrl("http://webview.active911.com/client/"));
    }
    if (calList.at(0).attributes().namedItem("enabled").nodeValue().toLower() == "false") {
        delete ui->m6TakeHomePage;
    }

    // start the timers up
    refreshTimer.start();
    progressTimer.start();
    redrawTimer.start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::refresh_timer()
{
    // Refresh data other than the signal/slot connected data
    QList<QPair<QString, QDateTime> > goodMessages;
    for (int i = 0; i < tickerMessages.count(); ++i)
    {
        if (tickerMessages.at(i).second > QDateTime::currentDateTime())
            goodMessages.append(tickerMessages.at(i));
    }
    tickerMessages = goodMessages;
    redraw();
}

void MainWindow::progress_timer()
{
    // Progress to the next screen
    int next = ui->stack->currentIndex()+1;
    if (next > ui->stack->count() - 1)
        next = 0;
    ui->stack->setCurrentIndex(next);
    int snap = next;
    while (ui->stack->currentWidget()->isHidden()) {
        ++next;
        if (next > ui->stack->count() - 1)
            next = 0;
        ui->stack->setCurrentIndex(next);
        if (snap == next)
            break; // prevent overflows
    }
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
    persistData();
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
    for (int i = 0; i < upcoming.count(); ++i) {
        if (upcoming.at(i)->start_date < QDateTime::currentDateTime()) {
            // Event is current, highlight
            QListWidgetItem* item = new QListWidgetItem(upcoming.at(i)->toString());
            item->setBackgroundColor(QColor(255, 255, 0, 128));
            ui->list_upcoming->addItem(item);
        } else
            ui->list_upcoming->addItem(upcoming.at(i)->toString());
    }
    // m6 takehome
    if (ui->m6TakeHomePage) {
        if (m6calendar == 0) {
            ui->list_m6takehome->clear();
            ui->list_m6takehome->addItem("There was an error");
            ui->list_m6takehome->addItem("loading the m6 calendar");
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
                        ui->list_m6takehome->addItem("There are no upcoming");
                        ui->list_m6takehome->addItem("shifts scheduled.");
                    }
                } else {
                    ui->list_m6takehome->addItem("No EMT currently assigned.");
                    ui->list_m6takehome->addItem(" ");
                    if (upcoming.count() == 1) {
                        ui->list_m6takehome->addItem(QString("Next: %1").arg(upcoming.at(0)->title));
                        ui->list_m6takehome->addItem(QString("On %1 at %2").arg(upcoming.at(0)->start_date.toString("ddd MMM dd")).arg(upcoming.at(0)->start_date.toString("hh:mm")));
                    } else {
                        ui->list_m6takehome->addItem("There are no upcoming");
                        ui->list_m6takehome->addItem("shifts scheduled.");
                    }
                }
            } else {
                ui->list_m6takehome->addItem(" ");
                ui->list_m6takehome->addItem("No shifts scheduled.");
            }
        }
    }
    // drills
    if (ui->upcomingDrillsPage) {
        ui->list_drills->clear();
        QList<CalendarEvent*> drills = drillCalendar->upcoming(6);
        for (int i = 0; i < drills.count(); ++i) {
            if (drills.at(i)->start_date < QDateTime::currentDateTime()) {
                // Event is current, highlight
                QListWidgetItem* item = new QListWidgetItem(drills.at(i)->drillFormat());
                item->setBackgroundColor(QColor(255, 255, 0, 128));
                ui->list_drills->addItem(item);
            } else if (drills.at(i)->isSpecialDrill()) {
                QListWidgetItem* item = new QListWidgetItem(drills.at(i)->drillFormat());
                item->setBackgroundColor(QColor(0, 255, 255, 128));
                ui->list_drills->addItem(item);
            } else
                ui->list_drills->addItem(drills.at(i)->drillFormat());
        }
    }

    // Ticker message
    QString totalMessage;
    for (int i = 0; i < tickerMessages.count(); ++i)
        totalMessage += tickerMessages.at(i).first + "  ---  ";
    totalMessage.chop(5);
    ui->scrollText->setText(totalMessage);

    // Center stuff
    for (int i = 0; i < ui->list_m6takehome->count(); ++i)
        ui->list_m6takehome->item(i)->setTextAlignment(Qt::AlignCenter);

    currentlyRedrawing = false;
}

void MainWindow::persistData()
{
#if __APPLE__
    // Stupid OS X
    QFile persistence("../../../persistence.dat");
#else
    QFile persistence("persistence.dat");
#endif
    persistence.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    QTextStream out(&persistence);
    for (int i = 0; i < tickerMessages.count(); ++i) {
        out << tickerMessages.at(i).second.toMSecsSinceEpoch() << endl;
        out << tickerMessages.at(i).first << endl;
    }
    persistence.flush();
    persistence.close();
}

bool MainWindow::loadPersistence()
{
#if __APPLE__
    // Stupid OS X
    QFile persistence("../../../persistence.dat");
#else
    QFile persistence("persistence.dat");
#endif
    try {
        persistence.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&persistence);
        while (!in.atEnd()) {
            bool ok = true;
            qint64 expirationEpoch = in.readLine().toLong(&ok);
            QString message = in.readLine();
            if (ok) {
                QDateTime expiration = QDateTime::fromMSecsSinceEpoch(expirationEpoch);
                if (expiration > QDateTime::currentDateTime())
                    tickerMessages.append(QPair<QString,QDateTime>(message, expiration));
            }
        }
        persistence.close();
        return true;
    } catch (std::exception &e) {
        qWarning() << "Unable to load persistence";
        qWarning() << e.what();
        return false;
    }
}
