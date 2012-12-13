#include "streammanager.h"
#include <QtXml>
#include <QFile>
#include <Logger.h>

StreamManager::StreamManager() :
    QObject(NULL)
{
    LOG_DEBUG("Creating stream manager\n");
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
        // timers should live in same thread with static image seeders
        newSeeder->metaObject()->invokeMethod(newSeeder,"doInit",Qt::QueuedConnection);
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
    LOG_DEBUG("Destroying stream manager\n");
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
     LOG_DEBUG("New seeder with oid = "+s+"\n");
     Q_ASSERT(s!="");
     StreamController * sc=NULL;
     if(activeControllers.contains(s))
     {
         LOG_DEBUG("Seeder with oid = "+s+"  - controller found\n");
         sc=activeControllers[s];
         if(qobject_cast<StaticImageSeeder*>(sc->seeder)!= NULL)
         {
             LOG_DEBUG("Seeder with oid = "+s+"  - controller has static seeder, replacing with new\n");
            reconnectSeeder(sc->seeder,seeder,sc->leechers);
            sc->seeder=seeder;
         }
         else
         {
             LOG_DEBUG("Seeder with oid = "+s+"  - controller has active seeder, moving to queue\n");
             sc->seederCandidates.append(seeder);
         }
     }
     else
     {
         LOG_DEBUG("Seeder with oid = "+s+"  - no controller, creating\n");
         sc= new StreamController();
         activeControllers[s]=sc;
         sc->seeder=seeder;
     }
     LOG_DEBUG("Seeder with oid = "+s+"  - adding done\n");

 }

void StreamManager::reconnectSeeder(AbstractSeeder* oldseeder,AbstractSeeder* newseeder,QList<Leecher*> leechers)
{
    LOG_DEBUG("StreamManager::reconnectSeeder -start");
    foreach (Leecher * l , leechers)
    {
        LOG_DEBUG("StreamManager::reconnectSeeder -disconnecting old");
        QObject::disconnect(oldseeder,SIGNAL(data(VideoFrame)),l,SLOT(data(VideoFrame)));
        LOG_DEBUG("StreamManager::reconnectSeeder -connecting new");
        QObject::connect(newseeder,SIGNAL(data(VideoFrame)),l,SLOT(data(VideoFrame)),Qt::QueuedConnection);
    }
    LOG_DEBUG("StreamManager::reconnectSeeder -success");
}






 void StreamManager::newLeecher(Leecher* leecher)
 {
     QMutexLocker l (&smMutex);
     QString s = leecher->getOid();
     LOG_DEBUG("new Leecher with oid = "+s+"\n");
     Q_ASSERT(s!="");
     StreamController * sc=NULL;
     if(activeControllers.contains(s))
     {
         LOG_DEBUG("Leecher with oid = "+s+"  - controller found\n");
         sc=activeControllers[s];
     }
     else
     {
         LOG_DEBUG("Leecher with oid = "+s+"  - no controller , creating\n");
         sc= new StreamController();
         activeControllers[s]=sc;
         QString tag = leecher->getImageTag();
         if(staticSeeders.contains(tag))
         {
             LOG_DEBUG("Leecher with oid = "+s+"  - found static image\n");
             sc->seeder=staticSeeders[tag];
         }
         else
         {
              LOG_DEBUG("Leecher with oid = "+s+"  - no static image? default used\n");
             sc->seeder=staticSeeders[defaultTag()];
         }
     }
     QObject::connect(sc->seeder,SIGNAL(data(VideoFrame)),leecher,SLOT(data(VideoFrame)),Qt::QueuedConnection);
     sc->leechers.append(leecher);
     LOG_DEBUG("Leecher with oid = "+s+"  - adding done\n");
 }

 void StreamManager::seederGone( AbstractSeeder* seeder)
 {
     QMutexLocker l (&smMutex);
     QString s = seeder->getOid();
     LOG_DEBUG("Removing seeder with oid = "+s+"\n");
     Q_ASSERT(s!="");
     StreamController * sc=NULL;
     if(activeControllers.contains(s))
     {
         LOG_DEBUG("Removing seeder with oid = "+s+" - controller found\n");
         sc=activeControllers[s];

         if(sc->seeder==seeder) // actual seeder
         {
             LOG_DEBUG("Removing seeder with oid = "+s+" - We are the main seeder\n");
             AbstractSeeder* sdr=NULL;

             if(!sc->seederCandidates.isEmpty())
             {
                  LOG_DEBUG("Removing seeder with oid = "+s+" - there are candidates\n");
                 sdr=sc->seederCandidates.takeFirst();
             }
             else
             {
                  LOG_DEBUG("Removing seeder with oid = "+s+" - no candidates\n");
                if(sc->leechers.size()>0)
                {
                    LOG_DEBUG("Removing seeder with oid = "+s+" - no candidates but we have leechers! Who will feed them?\n");
                    QString tag = sc->leechers[sc->leechers.size()-1]->getImageTag();
                    if(staticSeeders.contains(tag))
                    {
                         LOG_DEBUG("Removing seeder with oid = "+s+" - static image to replace seeder found\n");
                        sdr=staticSeeders[tag];
                    }
                    else
                    {
                        LOG_DEBUG("Removing seeder with oid = "+s+" - static image to replace seeder not found - using default\n");
                        sdr=staticSeeders[defaultTag()];
                    }
                }
                else
                {
                    LOG_DEBUG("Removing seeder with oid = "+s+" - no candidates and no leechers! \n");
                }
            }
            // we need change seeder
            if(sdr!= NULL)
            {
                LOG_DEBUG("Removing seeder with oid = "+s+" - replacing seeder with new one\n");
                reconnectSeeder(sc->seeder,sdr,sc->leechers);
                sc->seeder=sdr;
            }
            else
            {
                LOG_DEBUG("Removing seeder with oid = "+s+" - everybody gone, deleting controller\n");
                activeControllers.remove(s);
                sc->metaObject()->invokeMethod(sc, "deleteLater", Qt::QueuedConnection);
            }
        }
        else // candidate
        {
             LOG_DEBUG("Removing seeder with oid = "+s+" - we are just a candidate, so nothing to do here\n");
             sc->seederCandidates.removeAll(seeder);
        }
        LOG_DEBUG("Removing seeder with oid = "+s+" - success!\n");
     }else
     {
         LOG_ERROR("Removing seeder with oid = "+s+" - no controller for seeder!\n");
     }
 }

 void StreamManager::leecherGone(Leecher* leecher)
 {
     QMutexLocker l (&smMutex);
     QString s = leecher->getOid();
     LOG_DEBUG("Removing Leecher with oid = "+s+"\n");
     Q_ASSERT(s!="");
     StreamController * sc=NULL;
     if(activeControllers.contains(s))
     {
         LOG_DEBUG("Removing Leecher with oid = "+s+" - controller found\n");
         sc=activeControllers[s];
         sc->leechers.removeAll(leecher);
         if(sc->leechers.isEmpty())
         {
             LOG_DEBUG("Removing Leecher with oid = "+s+" - we are last leecher here\n");
             if(qobject_cast<StaticImageSeeder*>(sc->seeder)!= NULL)
             {
                 LOG_DEBUG("Removing Leecher with oid = "+s+" - seeder is static image - removing controller\n");
                 activeControllers.remove(s);
                 sc->metaObject()->invokeMethod(sc, "deleteLater", Qt::QueuedConnection);
             }
             else
             {
                 LOG_DEBUG("Removing Leecher with oid = "+s+" - seeder exists - keeping controller\n");
             }
         }
         else
         {
             LOG_DEBUG("Removing Leecher with oid = "+s+" - there is another Skywalker\n");
         }
     }
     else
     {
         LOG_ERROR("Removing Leecher with oid = "+s+" - no controller for Leecher!\n");
     }
 }

