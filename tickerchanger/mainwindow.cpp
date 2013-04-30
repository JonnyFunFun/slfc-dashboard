#include "mainwindow.h++"
#include "ui_mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QUrl>
#include <QNetworkAddressEntry>
#include <QDebug>
#include <QNetworkInterface>

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

void MainWindow::networkError(QNetworkReply::NetworkError networkError)
{
    qDebug() << networkError;
}

void MainWindow::addTickerMessage()
{
    ui->addButton->setEnabled(false);
    QNetworkAccessManager* nwam = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl(QString("http://192.168.0.254:18888/")));
    request.setRawHeader("X-Auth", "boobies");
    // date format: yyyy-MM-ddThh:mm:ss.zzzZ
    QString dateChunk = ui->inputExpiration->date().toString("yyyy-MM-ddThh:mm:ss.000Z");
    QString message = ui->inputMessage->text();
    QString payload(message+"=>"+dateChunk);
    QNetworkReply *reply = nwam->post(request, payload.toAscii());
    connect(reply, SIGNAL(finished()), this, SLOT(sendSuccessful()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void MainWindow::sendSuccessful()
{
    QMessageBox messageBox(QMessageBox::NoIcon, "Ticker Message Added Successfully", "Your ticker message was added successfully!", QMessageBox::Ok);
    messageBox.show();
    connect(&messageBox, SIGNAL(accepted()), this, SLOT(close()));
}
