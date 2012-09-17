#include "streampoint.h"


 StreamPoint::StreamPoint(QObject* parent)
     :QObject(parent),isDeleting(false)
 {}

void StreamPoint::prepareToDie()
{
    if(!isDeleting)
    {
        unregisterStreamPoint();
        deleteLater();
        isDeleting=true;
    }
}
