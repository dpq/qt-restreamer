#ifndef THREADPOOLWITHSIGNALS_H
#define THREADPOOLWITHSIGNALS_H

#include <QObject>

#include <QSet>
#include <QTimer>

class QThread;
class ControlThread;

class ThreadPoolWithSignals : public QObject
{
    Q_OBJECT
public:
    explicit ThreadPoolWithSignals(QObject *parent = 0);
    virtual ~ThreadPoolWithSignals();
     QThread* reserveThread();
public slots:

    void returnThread( QThread* ); // removes thread from active threads and adds it to inactive.
private:

    // deletes thread from inactive threads if any available.
    // void setThreadActive( QThread* ); // adds thread to active threads.
    QTimer cleanupTimer;
    //QScopedPointer<ControlThread> control;
    void cleanupThreads();// removes all threads from all lists
 private slots:
    void cleanupIdleThreads(); // removes, if any, all threads which are unused, left reserved threads only

private:
    QSet<QThread*> activeThreads;
    QSet<QThread*> idleThreads;
    QSet<QThread*> allThreads;
};

#endif // THREADPOOLWITHSIGNALS_H
