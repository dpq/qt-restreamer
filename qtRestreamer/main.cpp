#include <QtCore/QCoreApplication>

#include "requestlistener.h"
#include <Logger.h>
#include <ConsoleAppender.h>
#include <qhttpserver.h>
#include <QTcpSocket>


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
    bool res = s.listen(5001);
    LOG_INFO(QString("listen status: %1").arg(res));

    return a.exec();
}
