#include "requestlistener.h"
#include <Logger.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>
#include <QtWebSockets/QtWebSockets>


#include "leecher.h"
#include "mjpegstreamseeder.h"


RequestListener::RequestListener() :
    AbstractRequestHandler()
{

        LOG_INFO("RequestListener created");
}


void RequestListener::processResponce(QHttpRequest* req, QHttpResponse* resp)
{
    QString oid = QUrlQuery(req->url().query()).queryItemValue("oid");
    QString imageTag = QUrlQuery(req->url().query()).queryItemValue("imagetag");
    LOG_INFO("RequestListener catched connection of type "+req->method() + "with imagetag = "+ imageTag);
    if(oid=="")
    {
        resp->writeHead(404);
        resp->end("<html><head/><body></p>No token provided!</body></html>");
        LOG_INFO("RequestListener - no OID");
    }
    else
    {
        if(req->method()==QHttpRequest::GET)
        {
            Leecher* l =new Leecher(resp,oid,imageTag);

            resp->setHeader("Cache-Control", "no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0");
            resp->setHeader("Pragma", "no-cache");
            resp->setHeader("Connection", "keep-alive");
            resp->setHeader("Expires", "Mon, 3 Jan 2000 12:34:56 GMT");
            resp->setHeader("Content-Type", "multipart/x-mixed-replace;boundary="+AbstractSeeder::defaultBoundary);
            resp->setWriteTimeout(10000);
            l->incrementSocketBuffer(resp->writeHead(200));

            LOG_INFO("RequestListener - get connection");
        }
        else if (req->method()==QHttpRequest::POST)
        {
            new MJpegStreamSeeder(req,oid);
            req->setReadTimeout(5000);
            LOG_INFO("RequestListener - post connection");
        }
        else if(req->method()==QHttpRequest::TRACE)
        {

        }
        else
        {
            resp->writeHead(501);
            resp->end("<html><head/><body></p>Unsupported query type!</body></html>");
            LOG_INFO("RequestListener - huita connection");
        }
    }


    /*LOG_INFO("RequestListener catched request");
    resp->setHeader("Content-Length", "11");
    resp->writeHead(200);
    resp->write(QString("Hello World"));
    resp->end();*/

}

AbstractRequestHandler* makeRequestHandler()
{
    return new RequestListener();
}
