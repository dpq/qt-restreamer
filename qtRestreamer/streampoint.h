#ifndef STREAMPOINT_H
#define STREAMPOINT_H

#include <QString>

class StreamPoint
{
public:
    inline StreamPoint(){}
    virtual inline ~StreamPoint(){}
    const inline QString getOid() const {return oid;}
    inline void setOid(const QString& oid){this->oid=oid;}
private:
    QString oid;
};

#endif // STREAMPOINT_H
