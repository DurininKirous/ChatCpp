#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , trayIcon(new QSystemTrayIcon(this))
{
    ui->setupUi(this);
    User.socket = new QTcpSocket(this);
    connect(User.socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(User.socket, SIGNAL(disconnected()), this, SLOT(CloseSocket()));
    connect(User.socket, SIGNAL(connected()), this, SLOT(slotSendName()));
    ui->lineEdit_2->setText("127.0.0.1");
    ui->spinBox->setValue(2468);
    ui->lineEdit_3->setText("MyName");
    trayIcon.show();
    QMenu trayMenu;
    QAction *messageAction = trayMenu.addAction("New message");
    trayIcon.setContextMenu(&trayMenu);
    QObject::connect(&trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::ShowApp);
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
            ui->textBrowser->setTextColor(QColor(Qt::green));
            ui->textBrowser->append("System: You're connected!");
            ui->textBrowser->setTextColor(QColor(Qt::black));
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
            QTime time;
            in >> time;
            nextBlockSize=0;
            switch (command)
            {
                case (1):
                {
                    QString str;
                    in >> str;
                    str = time.toString() + " " + str;
                    ui->textBrowser->append(str);
                    ShowNotification("New message!", "You have a new message!");
                    break;
                }
            case 3:
            {
                QString str;
                in >> str;
                QMessageBox::information(this, "Error", str);
                User.Check=false;
                ui->pushButton_2->setText("Connect");
                User.socket->disconnectFromHost();
                break;
            }
            case 4:
            {
                if (User.socket->state() == QAbstractSocket::ConnectedState)
                    {
                        SaveFile();
                    }
                    else
                    {
                        // игнорировать получение файла
                        while (User.socket->bytesAvailable() > 0)
                        {
                            User.socket->readAll();     
                        }
                    }
                    break;
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
                str = time.toString() + " " + str;
                ui->textBrowser->append(str);
                ShowNotification("New message!", "You have a new message!");
                break;
            }
            case 7:
                User.socket->disconnectFromHost();
                break;
            case 9:
            {
                SaveFile();
                break;
            }
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
void MainWindow::SendFile(QString FilePath)
{
    Data.clear();
    QFile file(FilePath);
    file.open(QIODevice::ReadOnly);
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    QByteArray FileBase64 = file.readAll().toBase64();
    out << quint16(0) << User.commSendFileToEveryone << FileBase64;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    User.socket->write(Data);
    User.socket->waitForBytesWritten();
    file.close();
}
void MainWindow::SendFileToSpecificClient(QString FilePath, QString Name)
{
    Data.clear();
    QFile file(FilePath);
    file.open(QIODevice::ReadOnly);
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    QByteArray FileBase64 = file.readAll().toBase64();
    out << quint16(0) << User.commSendFileToSpecificClient << Name << FileBase64;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    User.socket->write(Data);
    User.socket->waitForBytesWritten();
    file.close();
}
void MainWindow::on_pushButton_clicked()
{
    if (User.socket->state() == QAbstractSocket::ConnectedState)
    {
        if (ui->listWidget->selectedItems().size()==0)
            SendToServer(ui->lineEdit->text(),User.commSendMessageToEveryone);
        else
        {
            for (auto Names: ui->listWidget->selectedItems())
            {
                SendToServerByName("*(private)* " + ui->lineEdit->text(),User.commSendMessageToSelectedUsersFromClient, Names->text());
            }
            ui->textBrowser->append(QTime::currentTime().toString() + " " + User.GetName()+": *(private)* " + ui->lineEdit->text());
        }
    }
    else
    {
        DisplayErrorMessageBox("The client is not connected, the message was not sent.");
    }
        ui->lineEdit->clear();
}


void MainWindow::on_lineEdit_returnPressed()
{
    if (User.socket->state() == QAbstractSocket::ConnectedState)
    {
        if (ui->listWidget->selectedItems().size()==0)
            SendToServer(ui->lineEdit->text(),User.commSendMessageToEveryone);
        else
        {
            for (auto Names: ui->listWidget->selectedItems())
            {
                SendToServerByName("*(private)* " + ui->lineEdit->text(),User.commSendMessageToSelectedUsersFromClient, Names->text());
            }
            ui->textBrowser->append(QTime::currentTime().toString() + " " + User.GetName()+": *(private)* " + ui->lineEdit->text());
        }
    }
    else
    {
        DisplayErrorMessageBox("The client is not connected, the message was not sent.");
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
    User.socket->deleteLater();
    ui->pushButton_2->setText("Connect");
    if (ui->listWidget->count() != 0) ui->listWidget->clear();
    User.Check=false;
    ui->textBrowser->setTextColor(QColor(Qt::red));
    ui->textBrowser->append("System: You're disconnected!");
    ui->textBrowser->setTextColor(QColor(Qt::black));
}

void MainWindow::on_pushButton_3_clicked()
{
    if (User.socket->state() == QAbstractSocket::ConnectedState)
    {
        QString Path = QFileDialog::getOpenFileName(this, "Select a file", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        if (!Path.isEmpty())
        {
            if (ui->listWidget->selectedItems().size()==0) SendFile(Path);
            else
            {
                for (auto Names: ui->listWidget->selectedItems())
                {
                    SendFileToSpecificClient(Path,Names->text());
                }
            }
        }
    }
    else
    {
        DisplayErrorMessageBox("The client is not connected and the file cannot be sent.");
    }
}
void MainWindow::SaveFile()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "New file!", "Someone sent you a file, do you want to accept it?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
    QString Path = QFileDialog::getSaveFileName(this, "Save file",QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    QFile file(Path);
    file.open(QIODevice::WriteOnly);
    QByteArray data = User.socket->readAll();
    data = QByteArray::fromBase64(data);
    file.write(data);
    file.close();
    }
    else
    {
        while (User.socket->bytesAvailable() > 0)
        {
            User.socket->readAll();
        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->listWidget->clearSelection();
}

void MainWindow::DisplayErrorMessageBox(QString str)
{
    QMessageBox::information(this, "Error!", str);
}

void MainWindow::ShowApp()
{
    this->setWindowState(Qt::WindowActive);
    this->activateWindow();
}

void MainWindow::ShowNotification(QString Heading, QString Body)
{
    if (this->isMinimized()) trayIcon.showMessage(Heading, Body, QSystemTrayIcon::Information, 10000);
}
