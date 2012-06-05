#ifndef LEECHER_H
#define LEECHER_H

#include <QObject>
#include <QList>
#include <QByteArray>
#include "streampoint.h"

class QHttpResponse;

class Leecher : public QObject,public StreamPoint
{
    Q_OBJECT
public:
    explicit Leecher(QHttpResponse* resp, QString oid);
    ~Leecher();
    inline void incrementSocketBuffer(qint64 sb){socketBuffer+=sb;}
signals:
protected:
    QHttpResponse* m_resp;
   // QByteArray rawBuffer;
    QList<QByteArray> frames;
    qint64 socketBuffer;
    static const int MAX_BUFFER;
    static const int MAX_FRAMES;

    void sendToSocket();
public slots:
   void data(QByteArray d);
   void bytesWritten(qint64 bw);
};

#endif // LEECHER_H
