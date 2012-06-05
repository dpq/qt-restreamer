#include "controlthread.h"

ControlThread::ControlThread(QObject *parent) :
    QThread(parent)
{
}

ControlThread::~ControlThread()
{
    if(isRunning())
        exit(0);
    if(!wait(3000))
        terminate();
}
