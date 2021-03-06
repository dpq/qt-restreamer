#ifndef ABSTRACTSEEDER_H
#define ABSTRACTSEEDER_H

#include <QObject>
#include "streampoint.h"
#include "videoframe.h"

class AbstractSeeder : public StreamPoint
{
    Q_OBJECT
public:
    explicit AbstractSeeder(QObject* parent=NULL);
    virtual ~AbstractSeeder(){}
    static const QString defaultBoundary;
protected:

signals:
    void data(VideoFrame d);
public slots:
    //void stop();
};

#endif // ABSTRACTSEEDER_H
