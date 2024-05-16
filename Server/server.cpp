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
            bool NameChanged=false;
            in >> str;
            nextBlockSize=0;
            for (auto it = Users.begin(); it != Users.end(); ++it)
            {
                if ((*it)->socket == qobject_cast<QTcpSocket*>(sender()))
                {
                    if (str.contains("Name:"))
                    {
                        str.remove(0,5);
                        (*it)->SetName(str);
                        NameChanged=true;
                    }
                    name=(*it)->GetName();
                }
            }
            if (NameChanged)
            {
                break;
            }
            str=name+": "+str;
            emit SendMessageToChat(str);
            SendMessageToClient(str);
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
void Server::SendMessageToClient(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << commSendMessageToEveryone << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    //socket -> write(Data);
    for(int i=0;i<Users.size();++i)
    {
        (*Users[i]).socket->write(Data);
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
    if (name.length() > 20 || name.length() < 5)
        return false;
    QRegularExpression r("[A-Za-z0-9_]+");
    return (r.match(name)).hasMatch();
}

bool Server::isntNameUsed(QString name) const
{
    for (int i = 0; i < Users.length(); ++i)
        if (Users.at(i)->GetName() == name)
            return false;
    return true;
}

