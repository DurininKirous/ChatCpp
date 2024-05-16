#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>
#include <QRegularExpression>
#include <QFile>
#include <client.h>
class Server: public QTcpServer
{
    Q_OBJECT
public:
    static const quint16 commSendMessageToEveryone=1;
public:
    Server();
    void StartServer(QHostAddress Addr, quint16 Port);
    void StopServer();
    void SendMessageToClient(QString str);
    void SendFileToClient(QString FilePath);
    bool isNameValid(QString name) const;
    bool isntNameUsed(QString name) const;
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
    void NameIsUsed();
    void NameIsntValid();
};
#endif // SERVER_H
