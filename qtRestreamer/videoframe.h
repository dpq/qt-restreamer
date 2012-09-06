#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

#include <QDateTime>

class VideoFrame
{
public:
    inline VideoFrame():data(),startTimestamp(QDateTime::currentMSecsSinceEpoch()){}
    inline VideoFrame(const QByteArray& dat):data(dat),startTimestamp(QDateTime::currentMSecsSinceEpoch()){}
    inline VideoFrame(const VideoFrame& other):data(other.data),startTimestamp(other.startTimestamp){}
    inline VideoFrame& operator =(const VideoFrame& other){data=other.data;startTimestamp=other.startTimestamp; return *this;}
    inline ~VideoFrame(){}
protected:
    QByteArray data;
    long startTimestamp;
public:
    inline QByteArray getData()const{return data;}
    inline void setData(QByteArray d)   {data=d;}
    inline long getTime()const{return startTimestamp;}
    inline void setTime(long time)   {startTimestamp=time;}
    inline long elapsed()const{return QDateTime::currentMSecsSinceEpoch()-startTimestamp;}
};



#endif // VIDEOFRAME_H
