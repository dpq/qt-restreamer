#ifndef ABSTRACTSEEDER_H
#define ABSTRACTSEEDER_H

#include <QObject>
#include "streampoint.h"

class AbstractSeeder : public QObject, public StreamPoint
{
    Q_OBJECT
public:
    explicit AbstractSeeder(QObject *parent = 0);
    virtual ~AbstractSeeder(){}
    static const QString defaultBoundary;
protected:

signals:
    void data(QByteArray d);
public slots:
    //void stop();
};

#endif // ABSTRACTSEEDER_H
