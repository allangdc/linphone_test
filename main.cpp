#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include "linphonecall.h"

#define ID_ALLAN    "sip:allangdc2@sip.linphone.org"
#define ID_ALLAN2   "sip:allangdc@sip.linphone.org"

int main(int argc, char *argv[])
{
    QString pwd;
    QCoreApplication a(argc, argv);
    MyLinphoneCall b;
    QTextStream qtin(stdin);
    qtin >> pwd;
    b.Init();
    b.SetParams("allangdc", "sip.linphone.org", "", pwd, "sip.linphone.org");
    b.Authentication();
    b.Call("sip:allangdc@sip.linphone.org");
    return a.exec();
}
