#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml/QtXml>
#include <QTimer>
#include "scrolltext.h++"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QTimer refreshTimer;
    QDomDocument configDocument;
    QList<QString> calendars;
    QString m6calendar;

private slots:
    virtual void refresh_timer();
};

#endif // MAINWINDOW_H
