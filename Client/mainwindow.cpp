#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    User.socket = new QTcpSocket(this);
    connect(User.socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(User.socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(User.socket, SIGNAL(connected()), this, SLOT(slotSendName()));
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
    User.SetName(ui->lineEdit_3->text());
    User.socket->connectToHost(Addr,Port);
}
void MainWindow::slotReadyRead()
{
    QDataStream in(User.socket);
    in.setVersion(QDataStream::Qt_6_2);
    if (in.status() == QDataStream::Ok)
    {
        /*QString str;
        in >> str;
        */
        for (;;)
        {
            if (nextBlockSize==0)
            {
                if (User.socket->bytesAvailable()<2)
                {
                    break;
                }
                in >> nextBlockSize;
            }
            if (User.socket->bytesAvailable() < nextBlockSize)
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
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    User.socket -> write(Data);
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
void MainWindow::slotSendName()
{
    QString name="Name:"+User.GetName();
    SendToServer(name);
}

