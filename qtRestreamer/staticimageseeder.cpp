#include "staticimageseeder.h"
#include <QFile>
#include <Logger.h>
#include <QCoreApplication>


StaticImageSeeder::StaticImageSeeder(QString imageTag, QString imagePath, QObject *parent) :
    AbstractSeeder(parent),m_imageTag(imageTag),t(this)
{
    //imageTag

    QFile f(imagePath);
    if(f.exists())
    {
        f.open(QIODevice::ReadOnly);
        oneImage=f.readAll();

    }
    else
    {
        LOG_ERROR("File with tag "+imageTag+" absent!!!! path is "+imagePath);
    }
  //void onTimeout();
   // this->moveToThread(QCoreApplication::instance()->thread());
    oneImage.prepend(QString("--"+defaultBoundary+"\r\nContent-Type: image/jpeg\r\nContent-Length:" + QString("%1").arg(oneImage.size()) + "\r\n\r\n").toLatin1());

}


void StaticImageSeeder::doInit()
{
    //t= new QTimer(this);
    connect(&t,SIGNAL(timeout()),this,SLOT(onTimeout()),Qt::QueuedConnection);

    t.start(200);
}

//const  QString StaticImageSeeder::defaultPath = "/var/www/WhitePixel45w.jpg";

void StaticImageSeeder::onTimeout()
{
    emit data(VideoFrame(oneImage));
}
