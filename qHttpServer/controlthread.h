#ifndef CONTROLTHREAD_H
#define CONTROLTHREAD_H

#include <QThread>

class ControlThread : public QThread
{
    Q_OBJECT
public:
    explicit ControlThread(QObject *parent = 0);
    ~ControlThread();
signals:
    
public slots:
    
};

#endif // CONTROLTHREAD_H
