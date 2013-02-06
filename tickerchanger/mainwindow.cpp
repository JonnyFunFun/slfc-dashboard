#include "mainwindow.h++"
#include "ui_mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addTickerMessage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTickerMessage()
{

}
