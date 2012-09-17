/*
 * Copyright 2011 Nikhil Marathe <nsm.nikhil@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE. 
 */

#include "qhttpconnection.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>
#include <QDebug>
#include <QSemaphore>
#include <QDateTime>

#include <sys/socket.h>

#include "qhttprequest.h"
#include "qhttpresponse.h"


QHttpConnection::QHttpConnection(QString serverDomain,int socketDescriptor, quint16 serverPort)
    : QObject(NULL)
    , m_socket(NULL)
    , initializing(true)
    , m_socketDescriptor(socketDescriptor)
    , m_serverPort(serverPort)
    , m_parser(0)
    , headerParced(false)
    , m_serverDomain(serverDomain)
    , socketWatcher(this)
    , lastRead(QDateTime::currentMSecsSinceEpoch())
    , lastWrite(QDateTime::currentMSecsSinceEpoch())
    , readTimeout(0)
    , writeTimeout(0)

{
   // qDebug() << "Got new connection" << socket->peerAddress() << socket->peerPort();

    m_parser = (http_parser*)malloc(sizeof(http_parser));
    http_parser_init(m_parser, HTTP_REQUEST);

    m_parserSettings.on_message_begin = MessageBegin;
    m_parserSettings.on_path = Path;
    m_parserSettings.on_query_string = 0;
   // m_parserSettings.on_query_string = QueryString;
    m_parserSettings.on_url = Url;
    m_parserSettings.on_fragment = Fragment;
    m_parserSettings.on_header_field = HeaderField;
    m_parserSettings.on_header_value = HeaderValue;
    m_parserSettings.on_headers_complete = HeadersComplete;
   // m_parserSettings.on_body = Body;
    m_parserSettings.on_message_complete = MessageComplete;

    m_parser->data = this;

}


 const QString QHttpConnection::flashPolicy=
         "<?xml version=\"1.0\"?>"\
         "<!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\">"\
         "<cross-domain-policy>"\
         "   <site-control permitted-cross-domain-policies=\"master-only\"/>"\
         "   <allow-access-from domain=\"%1\" to-ports=\"*\" />"\
         "</cross-domain-policy>";

//mcc.svoyaset.ru
void QHttpConnection::init()
{
    m_socket= new QTcpSocket(this);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(parseRequest()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));


    if (!m_socket->setSocketDescriptor(m_socketDescriptor))
    {
        qDebug()<<"Could not create socket from descriptor!";
        socketDisconnected();
    }
    else
    {
        //socketWatcher= new QTimer();
        //connect(this,SIGNAL(destroyed()),socketWatcher,SLOT(deleteLater()),Qt::DirectConnection);
        connect(&socketWatcher, SIGNAL(timeout()),this,SLOT(onControlTimerTimeout()),Qt::DirectConnection);
        connect(m_socket,SIGNAL(bytesWritten(qint64)),this,SLOT(onDataWrittenToSocket()),Qt::DirectConnection);
        connect(m_socket,SIGNAL(readyRead()),this,SLOT(onDataReadFromSocket()),Qt::DirectConnection);
        socketWatcher.start(50);
        qDebug() << "Got new connection" << m_socket->peerAddress() << m_socket->peerPort();

    }


}


void QHttpConnection::setReadTimeout(int milliSeconds)
{
    readTimeout=milliSeconds;

   /* if(m_socket!= NULL)
    {
        int descriptor = m_socket->socketDescriptor();

        struct timeval timeout;
        timeout.tv_sec = milliSeconds/1000;
        timeout.tv_usec = (milliSeconds%1000)*1000;
        if (setsockopt (descriptor, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0)
              qDebug()<<"Could not set socket read timeout!";

    }*/
}

void QHttpConnection::setWriteTimeout(int milliSeconds)
{
    writeTimeout=milliSeconds;
   /* if(m_socket!= NULL)
    {
        int descriptor = m_socket->socketDescriptor();

        struct timeval timeout;
        timeout.tv_sec = milliSeconds/1000;
        timeout.tv_usec = (milliSeconds%1000)*1000;
        if (setsockopt (descriptor, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0)
              qDebug()<<"Could not set socket read timeout!";

    }*/
}


void QHttpConnection::socketStateChanged(QAbstractSocket::SocketState state)
{
    switch(state)
    {
    case QAbstractSocket::UnconnectedState:
         qDebug()<<"Socket UnconnectedState";
         if(!initializing)
         {
             socketDisconnected();
         }
         break;
    case QAbstractSocket::BoundState:
         qDebug()<<"Socket BoundState";
         break;
    case QAbstractSocket::HostLookupState:
         qDebug()<<"Socket HostLookupState";
         break;
    case QAbstractSocket::ConnectingState:
         qDebug()<<"Socket ConnectingState";
         break;
    case QAbstractSocket::ConnectedState:
         qDebug()<<"Socket ConnectedState";
         initializing=false;
         break;
    case QAbstractSocket::ListeningState:
         qDebug()<<"Socket ListeningState";
         break;
    case QAbstractSocket::ClosingState:
         qDebug()<<"Socket ClosingState";
         break;
    }
;
}


QHttpConnection::~QHttpConnection()
{
    delete m_socket;
    m_socket = 0;

    free(m_parser);
    m_parser = 0;

}

void QHttpConnection::parseRequest()
{
    Q_ASSERT(m_parser);
    if(headerParced)
    {
        emit m_request->data(m_socket->read(m_socket->bytesAvailable()));
    }
    else
    {
        dataReaded.append(m_socket->read(m_socket->bytesAvailable()));
        if(dataReaded.startsWith("<policy-file-request/>"))
        {
         //   QString toSend = flashPolicy.arg(m_serverPort);
            m_socket->write(flashPolicy.arg(m_serverDomain).toAscii());
            m_socket->disconnectFromHost();
        }
        else
        {
            int headerEnd=dataReaded.indexOf("\r\n\r\n");
            if(headerEnd>0)
            {
                QByteArray header=dataReaded.mid(0,headerEnd+4);
                int nparsed = http_parser_execute(m_parser, &m_parserSettings, header.data(), header.size());
                if(nparsed>headerEnd)
                {
                     headerParced=true;
                     dataReaded=dataReaded.mid(nparsed);
                     emit m_request->data(dataReaded);
                     dataReaded.clear();
                }
            }
        }
    }
  /*  while()
    {
        QByteArray arr = m_socket->read(80*1024);

        if( arr.size() < 0 ) {
            // TODO
        }
        else {
            int nparsed = http_parser_execute(m_parser, &m_parserSettings, arr.data(), arr.size());
            if( nparsed != arr.size() ) {
            }
        }
    }*/
}

void QHttpConnection::forceClose()
{
    m_socket->abort();
}


qint64 QHttpConnection::write(const QByteArray &data)
{
    qint64 result=m_socket->write(data);
    if(result<0)
         m_socket->abort();
    return result;
}

void QHttpConnection::flush()
{
    m_socket->flush();
}

void QHttpConnection::responseDone()
{
    QHttpResponse *response = qobject_cast<QHttpResponse*>(QObject::sender());
    if( response->m_last )
    {
        m_socket->disconnectFromHost();
    }
}

void QHttpConnection::socketDisconnected()
{
    emit done(QThread::currentThread());
    metaObject()->invokeMethod(this, "deleteLater", Qt::QueuedConnection);
}

/********************
 * Static Callbacks *
 *******************/
int QHttpConnection::MessageBegin(http_parser *parser)
{
    QHttpConnection *theConnection = (QHttpConnection *)parser->data;
    theConnection->m_currentHeaders.clear();
    theConnection->m_request = new QHttpRequest(theConnection);
    connect(theConnection,SIGNAL( done(QThread*)),theConnection->m_request,SIGNAL(end()));
    connect(theConnection,SIGNAL( done(QThread*)),theConnection->m_request,SLOT(deleteLater()),Qt::QueuedConnection);
    return 0;
}

int QHttpConnection::HeadersComplete(http_parser *parser)
{
    QHttpConnection *theConnection = (QHttpConnection *)parser->data;
    Q_ASSERT(theConnection->m_request);

    /** set method **/
    QString method = QString::fromAscii(http_method_str((enum http_method) parser->method));
    theConnection->m_request->setMethod(method);

    /** set version **/
    theConnection->m_request->setVersion(QString("%1.%2").arg(parser->http_major).arg(parser->http_minor));

    // Insert last remaining header
    theConnection->m_currentHeaders[theConnection->m_currentHeaderField.toLower()] = theConnection->m_currentHeaderValue;

    /** set headers **/
    if( theConnection->m_currentHeaders.contains("host") ) {
        QUrl url = QUrl(theConnection->m_request->url());
        url.setAuthority(theConnection->m_currentHeaders["host"]);
        theConnection->m_request->setUrl(url);
    }
    else {
        // TODO: abort with 400
    }
    theConnection->m_request->setHeaders(theConnection->m_currentHeaders);

    /** set client information **/
    theConnection->m_request->m_remoteAddress = theConnection->m_socket->peerAddress().toString();
    theConnection->m_request->m_remotePort = theConnection->m_socket->peerPort();

    QHttpResponse *response = new QHttpResponse(theConnection);
    if( parser->http_major < 1 || parser->http_minor < 1 )
        response->m_keepAlive = false;
    connect(theConnection,SIGNAL( done(QThread*)),response,SLOT(end()));
    connect(response, SIGNAL(done()), theConnection, SLOT(responseDone()));
    connect(theConnection->m_socket,SIGNAL(bytesWritten(qint64)),response,SIGNAL(bytesWritten(qint64)));


    // we are good to go!
    emit theConnection->newRequest(theConnection->m_request, response);
    return 0;
}

int QHttpConnection::MessageComplete(http_parser *parser)
{
    Q_UNUSED(parser)
    // TODO: do cleanup and prepare for next request
   // QHttpConnection *theConnection = (QHttpConnection *)parser->data;
   // Q_ASSERT(theConnection->m_request);

   // emit theConnection->m_request->end();
    return 0;
}

int QHttpConnection::Path(http_parser *parser, const char *at, size_t length)
{

    QHttpConnection *theConnection = (QHttpConnection *)parser->data;
    Q_ASSERT(theConnection->m_request);
    QString path = QString::fromAscii(at, length);

    QUrl url = theConnection->m_request->url();
    url.setPath(path);
    theConnection->m_request->setUrl(url);
    return 0;
}

int QHttpConnection::QueryString(http_parser *parser, const char *at, size_t length)
{
    Q_UNUSED(at)
    Q_UNUSED(length)
    QHttpConnection *theConnection = (QHttpConnection *)parser->data;
    Q_ASSERT(theConnection->m_request);
   /* QString queryString = QString::fromAscii(at, length);

    QUrl url = theConnection->m_request->url();
    url.ad(queryString);
    theConnection->m_request->setUrl(url);*/
    Q_ASSERT(false);
    return 0;
}

int QHttpConnection::Url(http_parser *parser, const char *at, size_t length)
{
    qDebug() << "URL CALL" << QString::fromAscii(at, length);
    QHttpConnection *theConnection = (QHttpConnection *)parser->data;
    theConnection->m_request->m_url= QUrl::fromPercentEncoding(QByteArray(at, length));

    return 0;
}

int QHttpConnection::Fragment(http_parser *parser, const char *at, size_t length)
{
    Q_UNUSED(parser)
    Q_UNUSED(at)
    Q_UNUSED(length)
    Q_ASSERT(false);
    return 0;
}

int QHttpConnection::HeaderField(http_parser *parser, const char *at, size_t length)
{
    QHttpConnection *theConnection = (QHttpConnection *)parser->data;
    Q_ASSERT(theConnection->m_request);

    // insert the header we parsed previously
    // into the header map
    if( !theConnection->m_currentHeaderField.isEmpty() && !theConnection->m_currentHeaderValue.isEmpty() )
    {
        // header names are always lower-cased
        theConnection->m_currentHeaders[theConnection->m_currentHeaderField.toLower()] = theConnection->m_currentHeaderValue;
        // clear header value. this sets up a nice
        // feedback loop where the next time
        // HeaderValue is called, it can simply append
        theConnection->m_currentHeaderField = QString();
        theConnection->m_currentHeaderValue = QString();
    }

    QString fieldSuffix = QString::fromAscii(at, length);
    theConnection->m_currentHeaderField += fieldSuffix;
    return 0;
}

int QHttpConnection::HeaderValue(http_parser *parser, const char *at, size_t length)
{
    QHttpConnection *theConnection = (QHttpConnection *)parser->data;
    Q_ASSERT(theConnection->m_request);

    QString valueSuffix = QString::fromAscii(at, length);
    theConnection->m_currentHeaderValue += valueSuffix;
    return 0;
}

int QHttpConnection::Body(http_parser *parser, const char *at, size_t length)
{
    QHttpConnection *theConnection = (QHttpConnection *)parser->data;
    Q_ASSERT(theConnection->m_request);

    emit theConnection->m_request->data(QByteArray(at, length));
    return 0;
}

void QHttpConnection::onDataReadFromSocket()
{
    lastRead=QDateTime::currentMSecsSinceEpoch();
}

void QHttpConnection::onDataWrittenToSocket()
{
    lastWrite=QDateTime::currentMSecsSinceEpoch();
}

void QHttpConnection::onControlTimerTimeout()
{
    // don't worry. Exterminatus!
    if((readTimeout>0)&&((readTimeout+lastRead)<QDateTime::currentMSecsSinceEpoch()))
        m_socket->abort();
    if((writeTimeout>0)&&((writeTimeout+lastWrite)<QDateTime::currentMSecsSinceEpoch()))
        m_socket->abort();

}

