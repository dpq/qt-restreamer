#include <QtCore/QCoreApplication>
#include <QStringList>
#include <QTcpSocket>

#include "requestlistener.h"
#include <Logger.h>
#include <ConsoleAppender.h>
#include <qhttpserver.h>
#include "videoframe.h"
#include <iostream>




Q_DECLARE_METATYPE(QAbstractSocket::SocketState)
Q_DECLARE_METATYPE(VideoFrame)

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.addLibraryPath(a.applicationDirPath());
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<VideoFrame>("VideoFrame");
    ConsoleAppender* consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-7l] <%C> %m\n");
    QStringList args = QCoreApplication::arguments();

    int pos = args.indexOf("-l");
    if((pos>=0)&&(pos<args.size()-1))
    {
        consoleAppender->setDetailsLevel(args[pos+1]);
    }
    else
    {
        consoleAppender->setDetailsLevel(Logger::Debug);
    }
    Logger::registerAppender(consoleAppender);
    std::cout<<QString("Log level set to %1\n").arg(Logger::levelToString (consoleAppender->detailsLevel()) ).toStdString();
    LOG_INFO("Starting the application");


    QHttpServer s(&makeRequestHandler);

    pos = args.indexOf("-s");
    if((pos>=0)&&(pos<args.size()-1))
    {
        s.setServerDomain(args[pos+1]);
    }

    bool res = s.listen(5001);
    LOG_INFO(QString("listen status: %1").arg(res));

    return a.exec();
}
