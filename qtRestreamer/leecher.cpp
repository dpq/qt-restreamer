#include "leecher.h"
#include "streammanager.h"
#include <qhttpresponse.h>
#include <Logger.h>

Leecher::Leecher(QHttpResponse* resp, QString oid, QString imageTag) :
    QObject(NULL),StreamPoint(),socketBuffer(0),staticImageTag(imageTag)
{
    m_resp=resp;
    setOid(oid);
    //connect(request,SIGNAL(data(const QByteArray &)),this,SIGNAL(data(QByteArray)));
     connect(m_resp,SIGNAL(done()),this,SLOT(deleteLater()));
     connect(m_resp,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));

     if(staticImageTag.isEmpty())
     {
         staticImageTag=StreamManager::defaultTag();
     }

     StreamManager::instance()->newLeecher(this);
}


const int Leecher::MAX_BUFFER=65536;
const int Leecher::MAX_FRAMES=1;


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
            VideoFrame d=frames.takeFirst();
            socketBuffer+=d.getData().length();
            m_resp->write(d.getData());
            LOG_TRACE(QString("Frame passed. Size:%1 ,Time:%2").arg(d.getData().size()).arg(d.elapsed()));
        }
    }
}



 void Leecher::data(VideoFrame d)
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
