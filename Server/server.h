#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>
#include <QRegularExpression>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <client.h>
class Server: public QTcpServer
{
    Q_OBJECT
public:
    static const quint16 commSendMessageToEveryone=1;
    static const quint16 commSendUserName=2;
    static const quint16 commSendErrorMessageToSpecificClient=3;
    static const quint16 commSendFileToEveryone=4;
    static const quint16 commSendUsersOnline=5;
    static const quint16 commSendMessageToSpecificClient=6;
    static const quint16 commDisconnect=7;
    static const quint16 commGetMessageToSelectedUsersFromClient=8;
    static const quint16 commSendFileToSpecificClient=9;
public:
    Server();
    void StartServer(QHostAddress Addr, quint16 Port);
    void StopServer();
    void SendMessageToClient(QString str, quint16 comm);
    void SendErrorMessageToSpecificClient(Client* user, QString str);
    void SendMessageToSpecificClientByName(QString Name, QString str);
    void SendFileToClient(QString FilePath);
    void SendCommandToDisconnect(QString Name);
    void SendFile(QString FilePath);
    void SendFileToSpecificClient(QString FilePath, QString Name);
    QString GetStrOfUsers();
    bool isNameValid(QString name) const;
    bool isNameUsed(QString name) const;
    Client User;
    bool Check=false;
private:
    QVector<Client*> Users;
    QByteArray Data;
    quint16 nextBlockSize=0;


public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
    void slotDeleteUser();
signals:
    void SendMessageToChat(QString message);
    void SendGoodMessageToLogs(QString message);
    void SendBadMessageToLogs(QString message);
    void SendMessageToMessageBox(QString message);
    void AddUserToGui(QVector<Client*> Users);
    void UserIsDisconnected(QVector<Client*> Users);
    void ShowNotification(QString Heading, QString Body);
    void PathRequest();
    void NameIsUsed();
    void NameIsntValid();
};
#endif // SERVER_H
