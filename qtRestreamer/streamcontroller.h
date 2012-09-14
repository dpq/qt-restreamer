#ifndef STREAMCONTROLLER_H
#define STREAMCONTROLLER_H



#include "leecher.h"
#include "abstractseeder.h"

#include <QObject>
#include <QList>

class StreamManager;


class StreamController : public QObject
{
    Q_OBJECT
public:
    friend class StreamManager;
private:
    explicit StreamController(QObject *parent = 0);
    AbstractSeeder* seeder;
    QList<AbstractSeeder*> seederCandidates;
    QList<Leecher*> leechers;
signals:
    
public slots:
    
};

#endif // STREAMCONTROLLER_H
