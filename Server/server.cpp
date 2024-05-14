#include "server.h"
Server::Server() {};
void Server::StartServer(QHostAddress Addr, quint16 Port)
{
    if (this->listen(Addr,Port))
    {
        emit SendGoodMessageToLogs("System: The server started");
        Check=true;
    }
    else
    {
        emit SendBadMessageToLogs("System: Error, the server did not start working");
    }
}

void Server::StopServer()
{
    this->close();
    Check=false;
    emit SendGoodMessageToLogs("System: Server is closed...");
}
void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotDeleteUser()));

    Sockets.push_back(socket);
    QString message="System: Client connected "+QString::number(socketDescriptor);
    emit SendGoodMessageToLogs(message);
}
void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if (in.status() == QDataStream::Ok)
    {
        emit SendGoodMessageToLogs("System: Reading messages...");
        /*QString str;
        in >> str;
        qDebug()<<str;
        */
        for (;;)
        {
            if (nextBlockSize==0)
            {
                if (socket->bytesAvailable()<2)
                {
                    emit SendBadMessageToLogs("System: Data < 2, break");
                    break;
                }
                in >> nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize)
            {
                emit SendBadMessageToLogs("System: Data not full, break");
                break;
            }
            QString str;
            in >> str;
            nextBlockSize=0;
            emit SendMessageToChat(str);
            SendToClient(str);
            break;
        }
    }
    else
    {
        emit SendBadMessageToLogs("System: QDataStream error");
    }
}
void Server::slotDeleteUser()
{
    QTcpSocket* disconnectedSocket = qobject_cast<QTcpSocket*>(sender());
    if (disconnectedSocket)
    {
        Sockets.erase(std::remove(Sockets.begin(), Sockets.end(), disconnectedSocket), Sockets.end());
        disconnectedSocket->deleteLater();
        emit SendBadMessageToLogs("System: Client disconnected");
    }
}
void Server::SendToClient(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    //socket -> write(Data);
    for(int i=0;i<Sockets.size();++i)
    {
        Sockets[i]->write(Data);
    }
}
