/***************************************************************************
                          buffthread.cpp  -  description
                             -------------------
    begin                : Jun. 16 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <buffthread.h>

using namespace BuildRP;

BuffThread::BuffThread(QObject *parent):
QThread(parent)
{}

BuffThread::~BuffThread()
{}


void BuffThread::run()
{
    m_process(rp_control);
    emit resultReady(stReady);
}

void BuffThread::setProcess(process *sp,BuildRP::RPControl* rpctl,QString str)
{
    m_process= sp;
    rp_control = rpctl;
    stReady = str;
}
