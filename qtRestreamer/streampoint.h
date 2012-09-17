#ifndef STREAMPOINT_H
#define STREAMPOINT_H

#include <QString>
#include <QObject>
//#include "streammanager.h"


class StreamPoint: public QObject
{
    Q_OBJECT
public:
    StreamPoint(QObject* parent = NULL);
    virtual inline ~StreamPoint(){}
    const inline QString getOid() const {return oid;}
    inline void setOid(const QString& oid){this->oid=oid;}
private:
    QString oid;
    bool isDeleting;
public slots:
    void prepareToDie();
protected:
    virtual void unregisterStreamPoint()=0;

};

#endif // STREAMPOINT_H
