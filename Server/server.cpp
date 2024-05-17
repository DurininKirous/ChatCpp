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
        emit SendMessageToMessageBox("System: Error, the server did not start working. Possibly the port or address is unavailable.");
    }
}

void Server::StopServer()
{
    for (int i=0;i<Users.size();++i)
    {
        Users[i]->socket->disconnectFromHost();
        //Users[i]->socket->deleteLater();
        //Users[i]->socket->close();
    }
    Users.clear();
    this->close();
    Check=false;
    emit SendGoodMessageToLogs("System: Server is closed...");
}
void Server::incomingConnection(qintptr socketDescriptor)
{
    Client* newUser = new Client;
    newUser->socket = new QTcpSocket;
    newUser->socket->setSocketDescriptor(socketDescriptor);
    connect(newUser->socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(newUser->socket, SIGNAL(disconnected()), this, SLOT(slotDeleteUser()));
    //connect(newUser->socket, SIGNAL(SocketDisconnected()), this, SLOT(UserDisconnected()));

    Users.push_back(newUser);
    QString message = "System: Client connected " + QString::number(socketDescriptor);
    emit SendGoodMessageToLogs(message);
    qDebug() <<Users.size();
}
void Server::slotReadyRead()
{
    User.socket = (QTcpSocket*)sender();
    QDataStream in(User.socket);
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
                if (User.socket->bytesAvailable()<2)
                {
                    emit SendBadMessageToLogs("System: Data < 2, break");
                    break;
                }
                in >> nextBlockSize;
            }
            if (User.socket->bytesAvailable() < nextBlockSize)
            {
                emit SendBadMessageToLogs("System: Data not full, break");
                break;
            }
            QString str;
            QString name;
            quint16 comm;
            in >> comm;
            in >> str;
            nextBlockSize=0;
            switch (comm)
            {
            case 1:
                for (auto it = Users.begin(); it != Users.end(); ++it)
                {
                    if ((*it)->socket == qobject_cast<QTcpSocket*>(sender()))
                    {
                        name=(*it)->GetName();
                    }
                }
                str=name+": "+str;
                emit SendMessageToChat(str);
                SendMessageToClient(str, this->commSendMessageToEveryone);
                break;
            case 2:
                for (auto it = Users.begin(); it != Users.end(); ++it)
                {
                    if ((*it)->socket == qobject_cast<QTcpSocket*>(sender()))
                    {
                        if (!(this->isNameValid(str)))
                        {
                            SendMessageToSpecificClient((*it),"System: The name isn't valid. Please change it, you're disconnected");
                            //break;
                        }
                        else if (!(this->isNameUsed(str)))
                        {
                            SendMessageToSpecificClient((*it),"System: The name is already used. Please change it, you're disconnected");
                            //break;
                        }
                        else (*it)->SetName(str);
                        break;
                    }
                }
            }
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
    for (auto it = Users.begin(); it != Users.end(); ++it)
    {
        if ((*it)->socket == disconnectedSocket)
        {
            emit SendBadMessageToLogs("System: Client disconnected");
            (*it)->socket->deleteLater();
            delete (*it);
            Users.erase(it);
            break;
        }
    }
}
void Server::SendMessageToClient(QString str, quint16 comm)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << comm << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    //socket -> write(Data);
    for(int i=0;i<Users.size();++i)
    {
        (*Users[i]).socket->write(Data);
    }
}
void Server::SendMessageToSpecificClient(Client* user, QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << commSendMessageToSpecificClient << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    //socket -> write(Data);
    for(int i=0;i<Users.size();++i)
    {
        if ((*user).socket == (*Users[i]).socket)
        {
            (*Users[i]).socket->write(Data);
            break;
        }
    }
}
void Server::SendFileToClient(QString FilePath)
{
    QFile file(FilePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream out(&Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_2);
        out << quint16(0) << file.readAll();
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
    }
}
bool Server::isNameValid(QString name) const
{
    QRegularExpression r("^[A-Za-z0-9_]+$");
    return (r.match(name)).hasMatch();
}

bool Server::isNameUsed(QString name) const
{
    for (int i = 0; i < Users.length(); ++i)
        if (Users.at(i)->GetName() == name)
            return false;
    return true;
}

