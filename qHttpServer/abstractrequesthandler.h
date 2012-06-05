#ifndef ABSTRACTREQUESTHANDLER_H
#define ABSTRACTREQUESTHANDLER_H

#include <QObject>

class QHttpRequest;
class QHttpResponse;


class AbstractRequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit AbstractRequestHandler();// should NOT be parents



signals:
    
public slots:
    virtual void processResponce(QHttpRequest*,QHttpResponse*)=0;
};

typedef AbstractRequestHandler* (*RequestHandlerFactory)();

#endif // ABSTRACTREQUESTHANDLER_H
