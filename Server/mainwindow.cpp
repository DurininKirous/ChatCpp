#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    connect(&server,&Server::SendMessageToChat,this,&MainWindow::DisplayMessage);
    connect(&server,&Server::SendGoodMessageToLogs,this,&MainWindow::DisplayGoodLog);
    connect(&server,&Server::SendBadMessageToLogs,this,&MainWindow::DisplayBadLog);
    connect(&server, &Server::SendMessageToMessageBox,this,&MainWindow::DisplayErrorMessageBox);
    ui->setupUi(this);
    connect(ui->comboBox,&QComboBox::currentIndexChanged,this,&MainWindow::ChangeScreen);
    ui->stackedWidget->addWidget(ui->textBrowser);
    ui->stackedWidget->addWidget(ui->textBrowser_2);
    ui->stackedWidget->setCurrentWidget(ui->textBrowser);
    ui->lineEdit->setText("127.0.0.1");
    ui->spinBox->setValue(2468);
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
        if (server.Check == true )
        {
            ui->pushButton->setText("Stop server");
        }
    }
    else
    {
        server.StopServer();
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
/*void MainWindow::on_comboBox_activated(const QString &arg1)
{

}
*/
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
    server.SendMessageToClient(ui->lineEdit_2->text(),server.commSendMessageToEveryone);
    if (server.isListening())
    {
        ui->textBrowser->append(ui->lineEdit_2->text());
    }
    else
    {
        ui->textBrowser_2->setTextColor(QColor(Qt::red));
        ui->textBrowser_2->append("System: The server has not started yet, the message has not been sent");
        ui->textBrowser_2->setTextColor(QColor(Qt::black));
    }
    ui->lineEdit_2->clear();
}

void MainWindow::on_lineEdit_2_returnPressed()
{
    server.SendMessageToClient(ui->lineEdit_2->text(), server.commSendMessageToEveryone);
    if (server.isListening())
    {
        ui->textBrowser->append(ui->lineEdit_2->text());
    }
    else
    {
        ui->textBrowser_2->setTextColor(QColor(Qt::red));
        ui->textBrowser_2->append("System: The server has not started yet, the message has not been sent");
        ui->textBrowser_2->setTextColor(QColor(Qt::black));
    }
    ui->lineEdit_2->clear();
}
void MainWindow::DisplayErrorMessageBox(QString str)
{
    QMessageBox::information(this, "Error", str);
}
