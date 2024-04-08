#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>

class Server: QTcpServer
{
    Q_OBJECT
public:
    Server();
    void StartServer(QHostAddress Addr, quint16 Port);
    void StopServer();
    QTcpSocket* socket;
    bool Check=false;
private:
    QVector<QTcpSocket*> Sockets;
    QByteArray Data;
    void SendToClient(QString str);
    quint16 nextBlockSize=0;

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
    void slotDeleteUser();
};
#endif // SERVER_H
