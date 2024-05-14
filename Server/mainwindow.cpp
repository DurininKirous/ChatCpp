#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    connect(&server,&Server::SendMessageToWindow,this,&MainWindow::DisplayMessage);
    ui->setupUi(this);
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
    ui->pushButton->setText("Stop server");
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
