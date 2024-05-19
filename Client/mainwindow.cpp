#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    User.socket = new QTcpSocket(this);
    connect(User.socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(User.socket, SIGNAL(disconnected()), this, SLOT(CloseSocket()));
    connect(User.socket, SIGNAL(connected()), this, SLOT(slotSendName()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    if (User.Check == false)
    {
        QString Addr=ui->lineEdit_2->text();
        bool ok;
        quint16 Port=(ui->spinBox->text()).toInt(&ok);
        User.SetName(ui->lineEdit_3->text());
        User.socket->connectToHost(Addr,Port);
        User.socket->waitForConnected();
        if (User.socket->state() == QAbstractSocket::ConnectedState )
        {
            User.Check = true;
            ui->pushButton_2->setText("Disconnect");
        }
        else
        {
            QMessageBox::information(this, "Error", "The client has not connected, please check the connection parameters and server activity.");
        }
    }
    else
    {
        User.socket->disconnectFromHost();
        ui->pushButton_2->setText("Connect");
        if (ui->listWidget->count() != 0) ui->listWidget->clear();
        User.Check=false;
    }
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
                    qDebug() << "Data < 2, break\n";
                    break;
                }
                in >> nextBlockSize;
            }
            if (User.socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }
            quint16 command;
            in >> command;
            switch (command)
            {
                case (1):
                {
                    QString str;
                    in >> str;
                    nextBlockSize=0;
                    ui->textBrowser->append(str);
                    break;
                }
            case 3:
            {
                QString str;
                in >> str;
                nextBlockSize=0;
                QMessageBox::information(this, "Error", str);
                User.Check=false;
                ui->pushButton_2->setText("Connect");
                User.socket->disconnectFromHost();
                break;
            }
            case 4:
            {

            }
            case 5:
            {
                QString str;
                in >> str;
                QStringList Names = str.split('#');
                ui->listWidget->clear();
                for (QString name: Names)
                {
                    if (name != User.GetName()) ui->listWidget->addItem(name);
                }
                nextBlockSize=0;
                break;
            }
            case 6:
            {
                QString str;
                in >> str;
                nextBlockSize=0;
                ui->textBrowser->append(str);
                break;
            }
            case 7:
                User.socket->disconnectFromHost();
                ui->pushButton_2->setText("Connect");
                if (ui->listWidget->count() != 0) ui->listWidget->clear();
                User.Check=false;
            default:
                break;
            }
            break;
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
void MainWindow::SendToServer(QString str, quint16 comm)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << comm << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    User.socket -> write(Data);
}
void MainWindow::SendToServerByName(QString str, quint16 comm, QString Name)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << comm << Name << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    User.socket -> write(Data);
}
void MainWindow::on_pushButton_clicked()
{
    if (ui->listWidget->selectedItems().size()==0)
        SendToServer(ui->lineEdit->text(),User.commSendMessageToEveryone);
    else
    {
        for (auto Names: ui->listWidget->selectedItems())
        {
            SendToServerByName("*(private)* " + ui->lineEdit->text(),User.commSendMessageToSelectedUsersFromClient, Names->text());
        }
        ui->textBrowser->append(User.GetName()+": *(private)* " + ui->lineEdit->text());
    }
    ui->lineEdit->clear();
}


void MainWindow::on_lineEdit_returnPressed()
{
    if (ui->listWidget->selectedItems().size()==0)
        SendToServer(ui->lineEdit->text(),User.commSendMessageToEveryone);
    else
    {
        for (auto Names: ui->listWidget->selectedItems())
        {
            SendToServerByName("*(private)* " + ui->lineEdit->text(),User.commSendMessageToSelectedUsersFromClient, Names->text());
        }
        ui->textBrowser->append(User.GetName()+": *(private)* " + ui->lineEdit->text());
    }
    ui->lineEdit->clear();
}
void MainWindow::slotSendName()
{
    QString name=User.GetName();
    SendToServer(name, User.commSendUserName);
}
void MainWindow::CloseSocket()
{
    while (User.socket->state() != QAbstractSocket::UnconnectedState)
    //if (ui->listWidget->count() != 0) ui->listWidget->clear();
    User.socket->deleteLater();
}
