/***************************************************************************
                          overlayscene.cpp  -  description
                             -------------------
    begin                :Sep. 28 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "overlayscene.h"

using namespace RenderRP;


RPOverLayerScene::RPOverLayerScene(DataRP::RPData *rpdata):
    RPdraw(rpdata)
{
    rightBar = new ProgressBar(94.0f, 96.0f, 12.0f, 88.0f);
    leftBar = new ProgressBar(4.0f, 6.0f, 12.0f, 88.0f);
}

RPOverLayerScene::~RPOverLayerScene()
{
    delete rightBar;
    delete leftBar;
}

void RPOverLayerScene::draw()
{
    if(rp_data->getBar(DataRP::BAR_PRINT))
    {
        int maxlayer;

        maxlayer = rp_data->getMaxLevel();

        if(maxlayer <= 0)
           return;

        float printPercent = (float)rp_data->m_curPrintLevel/maxlayer;
        //right
        rightBar->setWindowSize(m_width,m_height);

        rightBar->setPercent(printPercent);

        rightBar->drawBar();
     }

     if(rp_data->getBar(DataRP::BAR_PROCESS))
     {
         float dataPercent =  rp_data->dataProcessPerc;
         leftBar->setWindowSize(m_width,m_height);
         leftBar->setPercent(dataPercent);
         leftBar->drawBar();

         if(dataPercent == 1.0f)
           rp_data->setBar(false,DataRP::BAR_PROCESS);
     };
}



