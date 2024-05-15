#ifndef CLIENT_H
#define CLIENT_H
#include <QObject>
#include <QTcpSocket>
class Client : public QObject
{
    Q_OBJECT
    QString Name="Unknown";
public:
    QTcpSocket*  socket;
    Client() {}
    ~Client();
    Client(const Client&) {}
    Client& operator=(const Client&) { return *this; }
    void SetName(QString name);
    QString GetName();
};

#endif // CLIENT_H
