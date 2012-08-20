#include "blanksquareseeder.h"
#include <QFile>
#include <Logger.h>
#include <QCoreApplication>


BlankSquareSeeder::BlankSquareSeeder(QObject *parent) :
    AbstractSeeder(parent)
{
    QFile f(defaultPath);
    if(f.exists())
    {
        f.open(QIODevice::ReadOnly);
        oneImage=f.readAll();

    }
    else
    {
        LOG_ERROR("File with blank square absent!!!!");
    }
  //void onTimeout();
   // this->moveToThread(QCoreApplication::instance()->thread());
    oneImage.prepend(QString("--"+defaultBoundary+"\r\nContent-Type: image/jpeg\r\nContent-Length:" + QString("%1").arg(oneImage.size()) + "\r\n\r\n").toAscii());
    connect(&t,SIGNAL(timeout()),this,SLOT(onTimeout()));
    t.start(200);
}

const  QString BlankSquareSeeder::defaultPath = "/var/www/WhitePixel45w.jpg";

void BlankSquareSeeder::onTimeout()
{
    emit data(VideoFrame(oneImage));
}
