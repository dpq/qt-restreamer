#include "streammanager.h"

StreamManager::StreamManager() :
    QObject(NULL)
{
    blankSeeder= new BlankSquareSeeder(NULL);
    blankSeeder->moveToThread(QCoreApplication::instance()->thread());
    this->moveToThread(QCoreApplication::instance()->thread());
}

StreamManager::~StreamManager()
{
    blankSeeder->deleteLater();
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
        QObject::disconnect(oldseeder,SIGNAL(data(QByteArray)),l,SLOT(data(QByteArray)));
        QObject::connect(newseeder,SIGNAL(data(QByteArray)),l,SLOT(data(QByteArray)),Qt::QueuedConnection);
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
         sc->seeder=blankSeeder;
     }
     QObject::connect(sc->seeder,SIGNAL(data(QByteArray)),leecher,SLOT(data(QByteArray)),Qt::QueuedConnection);
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
            reconnectSeeder(sc->seeder,blankSeeder,sc->leechers);
            sc->seeder=blankSeeder;
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
             if(sc->seeder==blankSeeder)
             {
                 activeControllers.remove(s);
                 sc->deleteLater();
             }
         }
     }
 }
