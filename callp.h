#ifndef CALLP_H
#define CALLP_H

#include <QObject>
#include <linphone/linphonecore.h>

class CallP : public QObject
{
    Q_OBJECT
private:
    QString _user;
    QString _password;
    QString _server;
    QString _serverPort;
    QString _proxy;
    QString _proxyPort;
    LinphoneCore *lincore;
public:
    explicit CallP(QObject *parent = 0);
    void SetParam(QString user,
                  QString password,
                  QString server,
                  QString serverPort,
                  QString proxy,
                  QString proxyPort);
    void Init();
    void Authenticate();
    void Call(QString id);
    void HangUp();
signals:

public slots:

};

#endif // CALLP_H
