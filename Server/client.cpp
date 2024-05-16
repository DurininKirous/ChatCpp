#include <client.h>
Client::Client() {}
Client::~Client()
{
    socket->deleteLater();
}
void Client::SetName(QString name)
{
    Name = name;
}
QString Client::GetName()
{
    return Name;
}
void Client::NameIsUsed()
{
    emit BoxNameIsUsed();
    socket->close();
}
void Client::NameIsntValid()
{
    emit BoxNameIsntValid();
    socket->close();
}
