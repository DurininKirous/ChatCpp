#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , ui(new Ui::MainWindow)
{
    connect(&server,&Server::SendMessageToChat,this,&MainWindow::DisplayMessage);
    connect(&server,&Server::SendGoodMessageToLogs,this,&MainWindow::DisplayGoodLog);
    connect(&server,&Server::SendBadMessageToLogs,this,&MainWindow::DisplayBadLog);
    connect(&server, &Server::SendMessageToMessageBox,this,&MainWindow::DisplayErrorMessageBox);
    connect(&server, &Server::AddUserToGui, this, &MainWindow::DisplayUsers);
    connect(&server, &Server::UserIsDisconnected, this, &MainWindow::DisplayUsers);
    connect(&server,&Server::PathRequest, this, &MainWindow::ChoosePath);
    ui->setupUi(this);
    connect(ui->comboBox,&QComboBox::currentIndexChanged,this,&MainWindow::ChangeScreen);
    connect(&server,&Server::ShowNotification,this,&MainWindow::ShowNotification);

    ui->stackedWidget->addWidget(ui->textBrowser);
    ui->stackedWidget->addWidget(ui->textBrowser_2);
    ui->stackedWidget->setCurrentWidget(ui->textBrowser);
    ui->lineEdit->setText("127.0.0.1");
    ui->spinBox->setValue(2468);
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

void MainWindow::on_pushButton_clicked()
{
    if (server.Check==false)
    {
        QHostAddress Addr;
        Addr.setAddress(ui->lineEdit->text());
        bool ok;
        quint16 Port=(ui->spinBox->text()).toInt(&ok);
        server.StartServer(Addr, Port);
        ui->textBrowser->setTextColor(QColor(Qt::green));
        ui->textBrowser->append("System: The server started!");
        ui->textBrowser->setTextColor(QColor(Qt::black));
        if (server.Check == true )
        {
            ui->pushButton->setText("Stop server");
        }
    }
    else
    {
        server.StopServer();
        ui->textBrowser->setTextColor(QColor(Qt::red));
        ui->textBrowser->append("System: Server is closed...");
        ui->textBrowser->setTextColor(QColor(Qt::black));
        ui->pushButton->setText("Start server");
    }
}
void MainWindow::DisplayMessage(QString message)
{
    ui->textBrowser->append(message);
}
void MainWindow::DisplayGoodLog(QString message)
{
    ui->textBrowser_2->setTextColor(QColor(Qt::green));
    ui->textBrowser_2->append(message);
    ui->textBrowser_2->setTextColor(QColor(Qt::black));
}

void MainWindow::DisplayBadLog(QString message)
{
    ui->textBrowser_2->setTextColor(QColor(Qt::red));
    ui->textBrowser_2->append(message);
    ui->textBrowser_2->setTextColor(QColor(Qt::black));
}

void MainWindow::ChangeScreen()
{
    if (ui->comboBox->currentIndex()==0)
    {
        ui->stackedWidget->setCurrentWidget(ui->textBrowser);
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->textBrowser_2);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (ui->listWidget->selectedItems().size()==0)
        server.SendMessageToClient("Server: " + ui->lineEdit_2->text(),server.commSendMessageToEveryone);
    else
    {
        for (auto Names: ui->listWidget->selectedItems())
        {
            server.SendMessageToSpecificClientByName(Names->text(),"Server: *(private)* " + ui->lineEdit_2->text());
        }
    }
    if (server.isListening())
    {
        if (ui->listWidget->selectedItems().size()==0)
            ui->textBrowser->append(QTime::currentTime().toString() + " Server: " + ui->lineEdit_2->text());
        else
            ui->textBrowser->append(QTime::currentTime().toString() + " Server: *(private)* " + ui->lineEdit_2->text());
    }
    else
    {
        ui->textBrowser_2->setTextColor(QColor(Qt::red));
        ui->textBrowser_2->append("System: The server has not started yet, the message has not been sent.");
        DisplayErrorMessageBox("The server has not started yet, the message has not been sent.");
        ui->textBrowser_2->setTextColor(QColor(Qt::black));
    }
    ui->lineEdit_2->clear();
}

void MainWindow::on_lineEdit_2_returnPressed()
{
    if (ui->listWidget->selectedItems().size()==0)
        server.SendMessageToClient("Server: " + ui->lineEdit_2->text(),server.commSendMessageToEveryone);
    else
    {
        for (auto Names: ui->listWidget->selectedItems())
        {
            server.SendMessageToSpecificClientByName(Names->text(),"Server: *(private)* " + ui->lineEdit_2->text());
        }
    }
    if (server.isListening())
    {
        if (ui->listWidget->selectedItems().size()==0)
            ui->textBrowser->append(QTime::currentTime().toString() + " Server: " + ui->lineEdit_2->text());
        else
            ui->textBrowser->append(QTime::currentTime().toString() + " Server: *(private)* " + ui->lineEdit_2->text());
    }
    else
    {
        ui->textBrowser_2->setTextColor(QColor(Qt::red));
        ui->textBrowser_2->append("System: The server has not started yet, the message has not been sent.");
        DisplayErrorMessageBox("The server has not started yet, the message has not been sent.");
        ui->textBrowser_2->setTextColor(QColor(Qt::black));
    }
    ui->lineEdit_2->clear();
}

void MainWindow::DisplayErrorMessageBox(QString str)
{
    QMessageBox::information(this, "Error!", str);
}

void MainWindow::DisplayUsers(QVector<Client*> Users)
{
    ui->listWidget->clear();
    for (int i=0; i < Users.size(); ++i)
    {
            ui->listWidget->addItem(Users[i]->GetName());
    }
}

void MainWindow::ChoosePath()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "New file!", "Someone sent you a file, do you want to accept it?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
    QString Path = QFileDialog::getSaveFileName(this, "Save file",QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    QFile file(Path);
    file.open(QIODevice::WriteOnly);
    QByteArray data = server.User.socket->readAll();
    data = QByteArray::fromBase64(data);
    file.write(data);
    file.close();
    }
    else
    {
        while (server.User.socket->bytesAvailable() > 0)
        {
            server.User.socket->readAll();
        }
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if (server.isListening())
    {
        QString Path = QFileDialog::getOpenFileName(this, "Select a file", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        if (!Path.isEmpty())
        {
            if (ui->listWidget->selectedItems().size()==0) server.SendFile(Path);
            else
            {
                for (auto Names: ui->listWidget->selectedItems())
                {
                    server.SendFileToSpecificClient(Path,Names->text());
                }
            }
        }
    }
    else
    {
        DisplayErrorMessageBox("The server is no started and the file cannot be sent.");
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    ui->listWidget->clearSelection();
}
void MainWindow::ShowNotification(QString Heading, QString Body)
{
    if (this->isMinimized()) trayIcon.showMessage(Heading, Body, QSystemTrayIcon::Information, 10000);
}
void MainWindow::ShowApp()
{
    this->setWindowState(Qt::WindowActive);
    this->activateWindow();
}
