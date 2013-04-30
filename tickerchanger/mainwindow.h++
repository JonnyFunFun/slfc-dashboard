#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

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
private slots:
    void addTickerMessage();
    void sendSuccessful();
    void networkError(QNetworkReply::NetworkError networkError);
};

#endif // MAINWINDOW_H
