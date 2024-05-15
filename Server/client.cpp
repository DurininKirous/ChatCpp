#include <client.h>
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
