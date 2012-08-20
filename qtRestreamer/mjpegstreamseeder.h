#ifndef MJPEGSTREAMSEEDER_H
#define MJPEGSTREAMSEEDER_H

#include "abstractseeder.h"
#include <QTimer>
#include <QTime>

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
    QTimer timeoutCheck;
    QTime time;
    int startTime;
signals:
    void end();
public slots:
    void onData(const QByteArray& data);
protected slots:
    void onTimeout();
};

#endif // MJPEGSTREAMSEEDER_H
