#ifndef MJPEGSTREAMSEEDER_H
#define MJPEGSTREAMSEEDER_H

#include "abstractseeder.h"

class QHttpRequest;

class MJpegStreamSeeder : public AbstractSeeder
{
    Q_OBJECT
public:
    explicit MJpegStreamSeeder(QHttpRequest *request,QString oid);
    ~MJpegStreamSeeder();
private:
    QHttpRequest * m_request;
    QByteArray boundary;
    QByteArray newBoundary;
    int searchStart;
    QByteArray localMiniBuffer;
signals:
    
public slots:
    void onData(const QByteArray& data);
};

#endif // MJPEGSTREAMSEEDER_H
