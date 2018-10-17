/***************************************************************************
                          buffthread.cpp  -  description
                             -------------------
    begin                : Sep. 23 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "datathread.h"

using namespace DataRP;

DataThread::DataThread(QObject *parent):
QThread(parent)
{}

DataThread::~DataThread()
{}


void DataThread::run()
{
    m_process(handle_line);
    emit resultReady(stReady);
}

void DataThread::setProcess(process *sp,DataRP::HandelScanPath *handelline,QString str)
{
    m_process= sp;
    handle_line = handelline;
    stReady = str;
}
