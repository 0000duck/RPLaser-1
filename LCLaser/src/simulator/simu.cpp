/***************************************************************************
                          simu.cpp  -  description
                             -------------------
    begin                : May 20 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "gtmock.h"
#include "graph.h"
#include "config.h"
#include <QDebug>

using namespace SIMURP;

RPsimu::RPsimu(QObject *parent): QObject(parent)
{
}


RPsimu::RPsimu(DataRP::RPData *rpdata,QObject *parent): QObject(parent),
    rp_data(rpdata), m_frameInterval(rp_data->getFrameInterval())
{
    qDebug()<<""<<endl;
}

RPsimu::~RPsimu()
{
}

