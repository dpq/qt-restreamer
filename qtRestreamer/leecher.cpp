#include "leecher.h"
#include "streammanager.h"
#include <qhttpresponse.h>
#include <Logger.h>

Leecher::Leecher(QHttpResponse* resp, QString oid) :
    QObject(NULL),StreamPoint(),socketBuffer(0)
{
    m_resp=resp;
    setOid(oid);
    //connect(request,SIGNAL(data(const QByteArray &)),this,SIGNAL(data(QByteArray)));
     connect(m_resp,SIGNAL(done()),this,SLOT(deleteLater()));
     connect(m_resp,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));
     StreamManager::instance()->newLeecher(this);
}


const int Leecher::MAX_BUFFER=65536;
const int Leecher::MAX_FRAMES=3;
const QString crlf = "\r\n";

Leecher::~Leecher()
{
     StreamManager::instance()->leecherGone(this);
}

void Leecher::bytesWritten(qint64 bw)
{
        socketBuffer-=bw;
        sendToSocket();
}

void Leecher::sendToSocket()
{
    if(socketBuffer<MAX_BUFFER)
    {
        if(frames.size())
        {
            QByteArray frame=frames.takeFirst();
            socketBuffer+=frame.length();
            m_resp->write(frame);
            socketBuffer+=2;
            m_resp->write(crlf);
        }
    }
}



 void Leecher::data(QByteArray d)
 {

     if(frames.size()<MAX_FRAMES)
     {
         frames.append(d);
     }
     else
     {
         frames[MAX_FRAMES-1]=d;
     }
    sendToSocket();

 }
