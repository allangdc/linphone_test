#ifndef BUDDYINFORMATION_H
#define BUDDYINFORMATION_H

#include <QObject>
#include <QDebug>
#ifdef IN_LINPHONE
#include "linphonecore.h"
#else
#include "linphone/linphonecore.h"
#endif

#include "mediastreamer2/mediastream.h"

#include <signal.h>
#include <unistd.h>

static bool lp_running=TRUE;

class MyLinphoneCall : public QObject
{
    Q_OBJECT
private:
    QString user;
    QString server;
    QString serverport;
    QString password;
    QString proxyaddress;
    LinphoneCore *lc = NULL;
    LinphoneCoreVTable vtable = {0};
    LinphoneCall *call = NULL;
    AudioStream *audiostream;

    static void stop(int signum);
    static void notify_presence_recv_updated (LinphoneCore *lc,  LinphoneFriend *_friend);
    static void new_subscription_request (LinphoneCore *lc,  LinphoneFriend *_friend, const char* url);
    static void registration_state_changed(struct _LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message);
    static void call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg);
public:
    explicit MyLinphoneCall(QObject *parent = 0);
    ~MyLinphoneCall();
    void Init();
    void SetParams(QString user,
                   QString server,
                   QString serverport,
                   QString password,
                   QString proxyaddress);
    void Call(QString dest);
    void Authentication();
signals:

public slots:

};

#endif // BUDDYINFORMATION_H
