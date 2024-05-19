#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <server.h>

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
    Server server;
public slots:
    void ChangeScreen();
    void ChoosePath();
private slots:
    void on_pushButton_clicked();
    void DisplayMessage(QString message);
    void DisplayGoodLog(QString message);
    void DisplayBadLog(QString message);
    void DisplayErrorMessageBox(QString message);
    void DisplayUsers(QVector<Client*> Users);
    //void on_comboBox_activated(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_lineEdit_2_returnPressed();
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
