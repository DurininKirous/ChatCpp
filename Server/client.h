#ifndef CLIENT_H
#define CLIENT_H
#include <QObject>
#include <QTcpSocket>
class Client : public QObject
{
    Q_OBJECT
    QString Name="Unknown";
public:
    static const quint16 commSendMessageToEveryone=1;
    static const quint16 commSendUserName=2;
    static const quint16 commSendErrorMessageToSpecificClient=3;
    static const quint16 commSendFileToEveryone=4;
    static const quint16 commSendUsersOnline=5;
    static const quint16 commSendMessageToSpecificClient=6;
    static const quint16 commDisconnect=7;
    static const quint16 commSendMessageToSelectedUsersFromClient=8;
    static const quint16 commSendFileToSpecificClient=9;
public:
    QTcpSocket* socket;
    bool Check=false;
public:
    Client();
    ~Client();
    void SetName(QString name);
    QString GetName();
public slots:
    void NameIsUsed();
    void NameIsntValid();
signals:
    void BoxNameIsUsed();
    void BoxNameIsntValid();
};

#endif // CLIENT_H
