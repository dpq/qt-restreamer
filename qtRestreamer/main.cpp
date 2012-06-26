#include <QtCore/QCoreApplication>
#include <QStringList>
#include <QTcpSocket>

#include "requestlistener.h"
#include <Logger.h>
#include <ConsoleAppender.h>
#include <qhttpserver.h>




Q_DECLARE_METATYPE(QAbstractSocket::SocketState)

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");

    ConsoleAppender* consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-7l] <%C> %m\n");
    consoleAppender->setDetailsLevel(Logger::Debug);
    Logger::registerAppender(consoleAppender);
    LOG_INFO("Starting the application");
    QHttpServer s(&makeRequestHandler);
    QStringList args = QCoreApplication::arguments();
    int pos = args.indexOf("-s");
    if((pos>=0)&&(pos<args.size()-1))
    {
        s.setServerDomain(args[pos+1]);
    }

    bool res = s.listen(5001);
    LOG_INFO(QString("listen status: %1").arg(res));

    return a.exec();
}
