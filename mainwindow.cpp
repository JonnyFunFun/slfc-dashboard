#include "mainwindow.h++"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtXml/QtXml>
#include <QFile>
#include <qmath.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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

    QDomNodeList calList = configDocument.documentElement().elementsByTagName("calendars").at(0).toElement().elementsByTagName("feed");
    qDebug() << QString("Opening %1 calendars...").arg(calList.count());

    if (calList.count() > 0)
    {
        for (int i = 0; i < calList.count(); i++)
        {
            calendars.append(QString(calList.at(i).attributes().namedItem("url").nodeValue()));
        }
    }

    // m6 takehome
    calList = configDocument.documentElement().elementsByTagName("m6takehome");
    if (calList.count() > 0)
    {
        m6calendar = QString(calList.at(0).attributes().namedItem("calendar").nodeValue());
        qDebug() << "Found M6 Takehome calendar.";
    } else {
        qWarning() << "No M6 Takehome calendar found.";
    }

    ui->setupUi(this);
    this->showFullScreen();
    this->setStyleSheet("background-color: black; color: white;");

    // Determine our constraints and calculate where our widgets are all going to be placed
    ui->scrollText->setGeometry(0, qCeil(height()*0.9), width(), qCeil(height()*0.1));
    ui->scrollText->setFixedHeight(qCeil(height()*0.1));

    // Our stack
    ui->stack->setGeometry(0, 0, width(), qFloor(height()*0.9));
    ui->stack->setStyleSheet("background-color: white!important; color: black!important;");

    // Page Labels
    ui->upcomingEventsLabel->setGeometry(0, 0, width(), qFloor(height()*0.08));
    ui->upcomingEventsLabel->setFont(QFont("Courier", height()*0.06));



    // Scrolling text
    ui->scrollText->setFont(QFont("Courier", height()*0.075));
    ui->scrollText->setText("Hello world! All your base are belong to us!");

    // Refresh timer
    connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh_timer()));
    refreshTimer.setInterval(3600000); // 1 hr
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh_timer()
{
    // Refresh our dataz
}
