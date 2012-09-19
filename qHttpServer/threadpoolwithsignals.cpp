#include "threadpoolwithsignals.h"
#include "controlthread.h"
#include <QDebug>
#include <QThread>
#include <QMutexLocker>

ThreadPoolWithSignals::ThreadPoolWithSignals(QObject *parent) :
    QObject(parent),cleanupTimer(this)/*,threadMutex(QMutex(QMutex::Recursive))*//*,control(new ControlThread(this))*/
{
    //cleanupTimer.setInterval(10000);
    connect(&cleanupTimer,SIGNAL(timeout()),this,SLOT(cleanupIdleThreads()));
    cleanupTimer.start(10000);
}


QThread* ThreadPoolWithSignals::reserveThread()
{
  //  QMutexLocker l(&threadMutex);
    QThread* t;
    if(!idleThreads.isEmpty())
    {
        t= *(idleThreads.begin());
        idleThreads.remove(t);
    }
    else
    {
        t = new ControlThread( this/*control.data()*/);
       // t->moveToThread(control.data());
        connect(t, SIGNAL(finished()), t, SLOT(deleteLater()), Qt::QueuedConnection);
        t->start();
        allThreads.insert(t);
    }
    activeThreads.insert(t);
    qDebug()<<"idle threads: "<<idleThreads.count()
            <<" active threads: "<<activeThreads.count()
           <<" total threads: "<<allThreads.count();
    return t;
}


void ThreadPoolWithSignals::returnThread( QThread* t) // removes thread from active threads and adds it to inactive.
{
  //  QMutexLocker l(&threadMutex);
    if(allThreads.contains(t))
    {
        activeThreads.remove(t);
        idleThreads.insert(t);
    }
    else
        qFatal("%s","ThreadPoolWithSignals::returnThread - not my thread!!!");
}

void ThreadPoolWithSignals::cleanupThreads()
{
   // QMutexLocker l(&threadMutex);
    QThread * t;
    foreach(t,allThreads)
    {
        t->quit();
    }
    allThreads.clear();
    idleThreads.clear();
    activeThreads.clear();
}

void ThreadPoolWithSignals::cleanupIdleThreads()
{
   // QMutexLocker l(&threadMutex);
    while (idleThreads.count()>5)
    {
        QThread* t = *(idleThreads.begin());
        idleThreads.remove(t);
        allThreads.remove(t);
        t->quit();
    }
}


ThreadPoolWithSignals::~ThreadPoolWithSignals()
{
    cleanupThreads();
}
