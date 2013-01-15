#include "mainwindow.h++"
#include "ui_mainwindow.h"

#include <QDebug>
#include <qmath.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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
    ui->upcomingEventsLabel->setGeometry(0, 0, width(), qFloor(height()*0.8));
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
