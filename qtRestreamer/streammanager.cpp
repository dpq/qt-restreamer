#include "streammanager.h"
#include <QtXml>
#include <QFile>

StreamManager::StreamManager() :
    QObject(NULL)
{
    QDomDocument domDoc;
    QFile domSrc(configPath);
    domSrc.open(QIODevice::ReadOnly);
    domDoc.setContent(&domSrc);
    QDomElement de= domDoc.documentElement().firstChildElement("image");
    do
    {

        QString tag= de.attribute("tag");
        QString path= de.attribute("path");
        StaticImageSeeder* newSeeder=new StaticImageSeeder(tag,path);
        newSeeder->moveToThread(QCoreApplication::instance()->thread());
        staticSeeders[tag]= newSeeder;

    }while (!(de=de.nextSiblingElement("image")).isNull());

    if(!staticSeeders.contains(defaultTag()))
    {
        qFatal("No default image found!!!");
    }


    //blankSeeder= new StaticImageSeeder(NULL);
    //blankSeeder->moveToThread(QCoreApplication::instance()->thread());
    this->moveToThread(QCoreApplication::instance()->thread());
}


QString StreamManager::configPath="/etc/QtRestreamerData/placeholders.xml";

StreamManager::~StreamManager()
{
    //blankSeeder->deleteLater();
    QList<StaticImageSeeder*> vals=staticSeeders.values();
    QListIterator<StaticImageSeeder*> it(vals);
    while (it.hasNext())
    {
        it.next()->deleteLater();
    }
}


StreamManager* StreamManager::m_instance=NULL;
QMutex StreamManager::smMutex;
//BlankSquareSeeder* StreamManager::blankSeeder;

 StreamManager * StreamManager::instance()
 {
     QMutexLocker l (&smMutex);
     if(!m_instance)
     {
         m_instance= new StreamManager();
     }
     return m_instance;
 }



 void StreamManager::newSeeder( AbstractSeeder* seeder)
 {
     QMutexLocker l (&smMutex);
     QString s = seeder->getOid();
     Q_ASSERT(s!="");
     StreamController * sc=NULL;
     if(activeControllers.contains(s))
     {
         sc=activeControllers[s];
         reconnectSeeder(sc->seeder,seeder,sc->leechers);
     }
     else
     {
         sc= new StreamController();
         activeControllers[s]=sc;
     }
      sc->seeder=seeder;
 }

void StreamManager::reconnectSeeder(AbstractSeeder* oldseeder,AbstractSeeder* newseeder,QList<Leecher*> leechers)
{
    foreach (Leecher * l , leechers)
    {
        QObject::disconnect(oldseeder,SIGNAL(data(VideoFrame)),l,SLOT(data(VideoFrame)));
        QObject::connect(newseeder,SIGNAL(data(VideoFrame)),l,SLOT(data(VideoFrame)),Qt::QueuedConnection);
    }
}






 void StreamManager::newLeecher(Leecher* leecher)
 {
     QMutexLocker l (&smMutex);
     QString s = leecher->getOid();
     Q_ASSERT(s!="");
     StreamController * sc=NULL;
     if(activeControllers.contains(s))
     {
         sc=activeControllers[s];
     }
     else
     {
         sc= new StreamController();
         activeControllers[s]=sc;
         QString tag = leecher->getImageTag();
         if(staticSeeders.contains(tag))
         {
             sc->seeder=staticSeeders[tag];
         }
         else
         {
             sc->seeder=staticSeeders[defaultTag()];
         }
     }
     QObject::connect(sc->seeder,SIGNAL(data(VideoFrame)),leecher,SLOT(data(VideoFrame)),Qt::QueuedConnection);
     sc->leechers.append(leecher);
 }

 void StreamManager::seederGone( AbstractSeeder* seeder)
 {
     QMutexLocker l (&smMutex);
     QString s = seeder->getOid();
     Q_ASSERT(s!="");
     StreamController * sc=NULL;
     if(activeControllers.contains(s))
     {
         sc=activeControllers[s];

         if(sc->leechers.size()>0)
         {
             QString tag = sc->leechers[0]->getImageTag();
             AbstractSeeder* sdr;
             if(staticSeeders.contains(tag))
             {
                 sdr=staticSeeders[tag];
             }
             else
             {
                 sdr=staticSeeders[defaultTag()];
             }
            reconnectSeeder(sc->seeder,sdr,sc->leechers);
            sc->seeder=sdr;
         }
         else
         {
             activeControllers.remove(s);
             sc->deleteLater();
         }

     }
 }

 void StreamManager::leecherGone(Leecher* leecher)
 {
     QMutexLocker l (&smMutex);
     QString s = leecher->getOid();
     Q_ASSERT(s!="");
     StreamController * sc=NULL;
     if(activeControllers.contains(s))
     {
         sc=activeControllers[s];
         sc->leechers.removeAll(leecher);
         if(sc->leechers.size()<=0)
         {
             if((staticSeeders.values().contains(qobject_cast<StaticImageSeeder*>(sc->seeder))))
             {
                 activeControllers.remove(s);
                 sc->deleteLater();
             }
         }
     }
 }
