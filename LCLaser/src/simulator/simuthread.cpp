/***************************************************************************
                          simuthread.cpp  -  description
                             -------------------
    begin                : Jun. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <simuthread.h>

using namespace SIMURP;

SimuThread::SimuThread(RPsimu *source, QObject *parent):
QThread(parent),rp_simu(source)
{}

SimuThread::~SimuThread()
{}


void SimuThread ::run()
{
    rp_simu->timeConsuming();
}
