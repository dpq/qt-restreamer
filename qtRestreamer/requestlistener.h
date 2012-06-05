#ifndef REQUESTLISTENER_H
#define REQUESTLISTENER_H

#include <QObject>
#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

class RequestListener : public AbstractRequestHandler
{
    Q_OBJECT
public:
    explicit RequestListener();

signals:
    
public slots:
    void processResponce(QHttpRequest* req, QHttpResponse* resp);
};

AbstractRequestHandler* makeRequestHandler();

#endif // REQUESTLISTENER_H
