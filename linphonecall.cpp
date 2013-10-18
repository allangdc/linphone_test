#include "linphonecall.h"

void MyLinphoneCall::stop(int signum)
{
    lp_running=FALSE;
}

void MyLinphoneCall::notify_presence_recv_updated (LinphoneCore *lc,  LinphoneFriend *_friend) {
        const LinphoneAddress* friend_address = linphone_friend_get_address(_friend);
        printf("New state state [%s] for user id [%s] \n"
                                ,linphone_online_status_to_string(linphone_friend_get_status(_friend))
                                ,linphone_address_as_string (friend_address));
}
void MyLinphoneCall::new_subscription_request (LinphoneCore *lc,  LinphoneFriend *_friend, const char* url) {
        const LinphoneAddress* friend_address = linphone_friend_get_address(_friend);
        printf(" [%s] wants to see your status, accepting\n"
                                ,linphone_address_as_string (friend_address));
        linphone_friend_edit(_friend); /* start editing friend */
        linphone_friend_set_inc_subscribe_policy(_friend,LinphoneSPAccept); /* Accept incoming subscription request for this friend*/
        linphone_friend_done(_friend); /*commit change*/
        linphone_core_add_friend(lc,_friend); /* add this new friend to the buddy list*/

}
void MyLinphoneCall::registration_state_changed(struct _LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message){
                printf("New registration state %s for user id [%s] at proxy [%s]\n"
                                ,linphone_registration_state_to_string(cstate)
                                ,linphone_proxy_config_get_identity(cfg)
                                ,linphone_proxy_config_get_addr(cfg));
}

void MyLinphoneCall::call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg)
{
    switch(cstate)
    {
    case LinphoneCallOutgoingRinging:
        printf("It is now ringing remotely!\n");
        break;
    case LinphoneCallOutgoingEarlyMedia:
        printf("Receiving some early media\n");
        break;
    case LinphoneCallOutgoingInit:
        printf("outgoing call is started\n");
        break;
    case LinphoneCallOutgoingProgress:
        printf("outgoing call is in progress\n");
        break;
    case LinphoneCallConnected:
        printf("We are connected!\n");
        break;
    case LinphoneCallStreamsRunning:
        printf("Media streams established!\n");
        break;
    case LinphoneCallEnd:
        printf("Call is terminated.\n");
        break;
    case LinphoneCallError:
        printf("Call failure!");
        break;
    default:
        printf("Unhandled notification %i\n",cstate);
    }
}

MyLinphoneCall::MyLinphoneCall(QObject *parent) :
    QObject(parent)
{
}

MyLinphoneCall::~MyLinphoneCall()
{
    printf("Shutting down...\n");
    if(lc)
        linphone_core_destroy(lc);
    printf("Exited\n");
}

/*!
 * \brief MyLinphoneCall::Init
 *
 * Inicializa o objeto linphonecore (lc)
 */
void MyLinphoneCall::Init()
{
    lp_running = TRUE;
    signal(SIGINT,stop);
    //#define DEBUG
#ifdef DEBUG
    linphone_core_enable_logs(NULL); /*enable liblinphone logs.*/
#endif
    vtable.call_state_changed=call_state_changed;
    vtable.notify_presence_recv=notify_presence_recv_updated;
    vtable.new_subscription_request=new_subscription_request;
    vtable.registration_state_changed=registration_state_changed;
    lc=linphone_core_new(&vtable,NULL,NULL,NULL);
    linphone_core_set_ring(lc, "/home/allan/Downloads/linphone-3.6.1/share/rings/oldphone.wav");
    linphone_core_set_ringback(lc, "/home/allan/Downloads/linphone-3.6.1/share/ringback.wav");
}

/*!
 * \brief MyLinphoneCall::SetParams
 * \param user Nome do usuario
 * \param server Nome do servidor
 * \param serverport Porta do servidor
 * \param password Senha do usuario
 * \param proxyaddress Endereco do proxy
 *
 * Seta as variaveis que serao utilizadas na autenticaçao
 */
void MyLinphoneCall::SetParams(QString user, QString server, QString serverport, QString password, QString proxyaddress)
{
    this->user=user;
    this->server = server;
    this->serverport = serverport;
    this->password = password;
    this->proxyaddress = proxyaddress;
}

/*!
 * \brief MyLinphoneCall::Call
 * \param dest
 */
void MyLinphoneCall::Call(QString dest)
{
    if (!dest.isEmpty())
    {
        const char *cdest = dest.toLocal8Bit();
        call=linphone_core_invite(lc,cdest);
        if(call==NULL)
        {
            printf("Could not place call to %s\n",cdest);
            exit(1);
        }
        else
            printf("Call to %s is in progress...",cdest);
        linphone_call_ref(call);
        printf("\n\nAQUI\n\n");
        float x = linphone_call_get_record_volume(call);
        qDebug() << x;
        while(lp_running)
        {
            linphone_core_iterate(lc);
            usleep(50000);
        }

        if(call && linphone_call_get_state(call)!=LinphoneCallEnd)
        {
            /* terminate the call */
            printf("Terminating the call...\n");
            linphone_core_terminate_call(lc,call);
            /*at this stage we don't need the call object */
            linphone_call_unref(call);
        }
    }
}

/*!
 * \brief MyLinphoneCall::Authentication
 */
void MyLinphoneCall::Authentication()
{
    QString identity;
    if(serverport.isEmpty())
        identity = QString("sip:%1@%2").arg(user).arg(server);
    else
        identity = QString("sip:%1@%2:%3").arg(user).arg(server).arg(serverport);
    qDebug() << "***" << identity << "***";
    LinphoneAddress *from = linphone_address_new(identity.toLocal8Bit());
    LinphoneAuthInfo *info = linphone_auth_info_new(linphone_address_get_username(from),
                                                    //user.toLocal8Bit(),
                                                    NULL,
                                                    password.toLocal8Bit(),
                                                    NULL,
                                                    NULL);
    linphone_address_destroy(from);
    linphone_core_add_auth_info(lc, info);

    LinphoneProxyConfig *proxycfg = linphone_proxy_config_new();
    linphone_proxy_config_set_identity(proxycfg, identity.toLocal8Bit());
    linphone_proxy_config_set_server_addr(proxycfg, proxyaddress.toLocal8Bit());
    linphone_proxy_config_enable_register(proxycfg, TRUE);
    linphone_core_add_proxy_config(lc, proxycfg);
    linphone_core_set_default_proxy(lc, proxycfg);

    /* Loop until registration status is available */
    do {
        linphone_core_iterate(lc); /* first iterate initiates registration */
        usleep(100000);
    }
    while(  lp_running && linphone_proxy_config_get_state(proxycfg) == LinphoneRegistrationProgress);
}
