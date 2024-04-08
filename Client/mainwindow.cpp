#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    QString Addr=ui->lineEdit_2->text();
    bool ok;
    quint16 Port=(ui->spinBox->text()).toInt(&ok);
    socket->connectToHost("127.0.0.1",1234);
}
void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_6);
    if (in.status() == QDataStream::Ok)
    {
        /*QString str;
        in >> str;
        */
        for (;;)
        {
            if (nextBlockSize==0)
            {
                if (socket->bytesAvailable()<2)
                {
                    break;
                }
                in >> nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }
            QString str;
            in >> str;
            nextBlockSize=0;
            ui->textBrowser->append(str);
        }
    }
    else
    {
        ui->textBrowser->append("read error...");
    }
}
/*void MainWindow::slotDeleteUser()
{
    Sockets.erase(std::remove_if(Socket.begin(), Socket.end(), socket), Socket.end());
    Socket->deleteLater();
}*/
void MainWindow::SendToServer(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket -> write(Data);
}

void MainWindow::on_pushButton_clicked()
{
    SendToServer(ui->lineEdit->text());
    ui->lineEdit->clear();
}


void MainWindow::on_lineEdit_returnPressed()
{
    SendToServer(ui->lineEdit->text());
    ui->lineEdit->clear();
}

