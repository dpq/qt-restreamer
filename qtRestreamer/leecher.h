#ifndef LEECHER_H
#define LEECHER_H

#include <QObject>
#include <QList>
#include <QByteArray>
#include "streampoint.h"
#include "videoframe.h"

class QHttpResponse;

class Leecher : public StreamPoint
{
    Q_OBJECT
public:
    explicit Leecher(QHttpResponse* resp, QString oid, QString imageTag);
    ~Leecher();
    inline void incrementSocketBuffer(qint64 sb){socketBuffer+=sb;}
    inline const QString getImageTag()const {return staticImageTag;}

//    inline const bool getDeletingStatus(){return isDeleting;}

protected:
    virtual void unregisterStreamPoint();
signals:
protected:
    QHttpResponse* m_resp;
   // QByteArray rawBuffer;
    QList<VideoFrame> frames;
    qint64 socketBuffer;
    static const int MAX_BUFFER;
    static const int MAX_FRAMES;
    QString staticImageTag;
    void sendToSocket();
    //bool isDeleting;
public slots:
   void data(VideoFrame d);
   void bytesWritten(qint64 bw);
};

#endif // LEECHER_H
