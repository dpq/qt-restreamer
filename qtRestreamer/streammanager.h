#ifndef STREAMMANAGER_H
#define STREAMMANAGER_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>
#include "streamcontroller.h"
#include "blanksquareseeder.h"
#include "abstractseeder.h"
#include "leecher.h"


class StreamManager : public QObject
{
    Q_OBJECT
public:

    static StreamManager * instance();
signals:
protected:
    StreamManager();
    ~StreamManager();

    QMap<QString,StreamController*> activeControllers;
    BlankSquareSeeder* blankSeeder;
    static StreamManager* m_instance;
    static QMutex smMutex;
public:
    void newSeeder( AbstractSeeder* seeder);
    void newLeecher( Leecher* leecher);
    void seederGone( AbstractSeeder* seeder);
    void leecherGone( Leecher* leecher);

private:
    void reconnectSeeder(AbstractSeeder* oldseeder,AbstractSeeder* newseeder,QList<Leecher*> leechers);


};

#endif // STREAMMANAGER_H
