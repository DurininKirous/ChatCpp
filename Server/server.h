#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>

class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server();
    void StartServer(QHostAddress Addr, quint16 Port);
    void StopServer();
    void SendToClient(QString str);
    QTcpSocket* socket;
    bool Check=false;
private:
    QVector<QTcpSocket*> Sockets;
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
};
#endif // SERVER_H
