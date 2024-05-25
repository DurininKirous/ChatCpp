#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFile>
#include <QFileDialog>
#include <client.h>
#include <QThread>
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
    void SendToServer(QString str, quint16 comm);
    void SendToServerByName(QString str, quint16 comm, QString Name);
    void SendFile(QString FilePath);
    void SendFileToSpecificClient(QString FilePath, QString Name);
    void SaveFile();
    void DisplayErrorMessageBox(QString str);
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    Client User;
    QByteArray Data;
    quint16 nextBlockSize=0;
public slots:
    void  slotReadyRead();
    void  slotSendName();
    void CloseSocket();
};
#endif // MAINWINDOW_H
