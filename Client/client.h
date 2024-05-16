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
public:
    QTcpSocket* socket;
    bool Check=false;
    Client();
    ~Client();
    Client(const Client&) {}
    Client& operator=(const Client&) { return *this; }
    void SetName(QString name);
    QString GetName();
public slots:
    void NameIsUsed();
    void NameIsntValid();
    //void ReadFile();
signals:
    void BoxNameIsUsed();
    void BoxNameIsntValid();
};

#endif // CLIENT_H
