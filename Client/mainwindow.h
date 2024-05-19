#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QFile>
#include <client.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_lineEdit_returnPressed();

private:
    Ui::MainWindow *ui;
    Client User;
    QByteArray Data;
    void SendToServer(QString str, quint16 comm);
    void SendToServerByName(QString str, quint16 comm, QString Name);
    quint16 nextBlockSize=0;
public slots:
    void  slotReadyRead();
    void  slotSendName();
    void CloseSocket();
};
#endif // MAINWINDOW_H
