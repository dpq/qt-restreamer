#include "mjpegstreamseeder.h"
#include "streammanager.h"
#include <qhttprequest.h>
#include <Logger.h>

MJpegStreamSeeder::MJpegStreamSeeder(QHttpRequest *request,QString oid) :
    AbstractSeeder(),searchStart(0),startTime(QDateTime::currentMSecsSinceEpoch())
{
    m_request=request;
    setOid(oid);
    connect(request,SIGNAL(data(const QByteArray &)),this,SLOT(onData(QByteArray)));
    connect(request,SIGNAL(end()),this,SLOT(prepareToDie()), Qt::QueuedConnection);
    //connect(this,SIGNAL(end()),request,SIGNAL(end()));

    QByteArray ctype = request->header("content-type").toAscii();
    int bndnameid=ctype.indexOf("boundary=");
    if(bndnameid>0)
    {
        boundary=ctype.mid(bndnameid+9/*sizeof boundary='*/);
    }
    if(boundary.length())
    {
        boundary=boundary.prepend("--");
        boundary=boundary.append("\r\n");
    }
    newBoundary=defaultBoundary.toAscii();
    newBoundary=newBoundary.prepend("--");
    newBoundary=newBoundary.append("\r\n");
    if(boundary==newBoundary)
    {
        searchStart=newBoundary.length();
    }
    StreamManager::instance()->newSeeder(this);
    connect(&timeoutCheck, SIGNAL(timeout()),this,SLOT(onTimeout()));
    timeoutCheck.start(1000);
    time.start();

}

void MJpegStreamSeeder::unregisterStreamPoint()
{
    StreamManager::instance()->seederGone(this);
}


MJpegStreamSeeder::~MJpegStreamSeeder()
{

}

void MJpegStreamSeeder::onTimeout()
{
    if(time.elapsed()>10000)
        emit end();
}


void MJpegStreamSeeder::onData(const QByteArray& dat)
{
    time.restart();
    localMiniBuffer.append(dat);
    if(boundary.length())
    {
        int bnd = localMiniBuffer.indexOf(boundary,searchStart);
        if(bnd>=0)
        {
            QByteArray frame =  localMiniBuffer.mid(0,bnd);
            if(frame.length())
            {
                //LOG_TRACE("Data: "+QString(frame.mid(0,10))+"..."+QString(frame.mid(frame.length()-11)));
                VideoFrame realFrame(frame);
                realFrame.setTime(startTime);
                startTime=QDateTime::currentMSecsSinceEpoch();
                 emit data(realFrame);
                localMiniBuffer=localMiniBuffer.mid(bnd);
            }
            localMiniBuffer=localMiniBuffer.replace(0,boundary.length(),newBoundary);
        }
    }
    else
    {
        LOG_ERROR("No boundary in seeder!!!");
        startTime=QDateTime::currentMSecsSinceEpoch();
        emit data(VideoFrame(dat));
    }
}
