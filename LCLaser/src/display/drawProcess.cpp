/***************************************************************************
                             drawProcess.cpp  -  description
                             -------------------
    begin                : Jul. 3 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QTimerEvent>
#include <QDebug>
#include "drawProcess.h"

using namespace RenderRP;


RPDrawProcess::RPDrawProcess():
    m_precursor(0),m_cursor(1),
    m_start(0),m_list_Cursor(0),
    b_cursor(false),b_showBalls(false),b_showLinks(false),b_contactLine(false)
{
    m_biLine.setShow(false);
}

RPDrawProcess::RPDrawProcess(DataRP::RPData *rpData):
    RPdraw(rpData),
    m_precursor(0),m_cursor(1),
     m_start(0),m_list_Cursor(0),
    b_cursor(false),b_showBalls(false),b_showLinks(false),b_contactLine(false)
{
    m_simuBall = rp_data->m_simuBall;
    m_biLine.setShow(false);
}


RPDrawProcess::~RPDrawProcess()
{
}

void RPDrawProcess::listen()
{
     m_timer.start(rp_data->getFrameInterval()*10,this);
}

void RPDrawProcess::listenOK()
{
     m_timer.stop();
     finishAll();
}

bool RPDrawProcess::canbeDisplay()
{
    if(!rp_data->isDrawProcess())
        return false;

    return true;
}

void RPDrawProcess::reset()
{
    m_precursor = 0;
    m_cursor = 1;
    m_start = 0;
    m_list_Cursor = 0;
}


/****************draw******************/
void RPDrawProcess::draw()
{
    if(!canbeDisplay())
        return;

    drawBalls();
    drawCursor();
    drawLines();

}

 void RPDrawProcess::drawBalls()
{

     if(!b_showBalls)
         return;

     RPDisk ball;
     DataRP::RPColor color;
     unsigned length = m_simuBall->m_animData.balls.size();

     if(length < 1)
      return;


     for(unsigned i = 0; i < m_cursor+1; i++)  // m_cursor+1  length.
     {
         ball =m_simuBall->m_animData.balls[i];

         switch(ball.getStatus())
         {
             case DataRP::DR_BALL_NORMAL:
                 color = DataRP::RPColor(0.0,1.0,1,0);
             break;
             case DataRP::DR_BALL_DELETED:
                 color = DataRP::RPColor(1.0,0.0,0,0);
             break;
             case DataRP::DR_BALL_MOVEING:
                 color = DataRP::RPColor(1.0,0.0,1,0);
             break;
             case DataRP::DR_BALL_ALTMOVEING:
                 color = DataRP::RPColor(1.0,1.0,0,0);
             break;
             case DataRP::DR_BALL_CURRENT:
                 color = DataRP::RPColor(0.0,1.0,0,0);
             break;
             default:
                 color = DataRP::RPColor(1.0,1.0,1,0);
             break;
         }

         Graphs::drawDisk(ball.getCenter(),ball.getRadius(),24,color);
     }
 }

 void RPDrawProcess::drawCursor()
 {
     if(!b_cursor)
        return;
     //draw cursor.
      unsigned limit = m_simuBall->m_animData.cursors.size();

      if(limit < 1 )
          return;

      if(m_precursor > limit || m_cursor > limit)
          return;

     Graphs::drawArrow(m_simuBall->m_animData.cursors[m_precursor],DataRP::RPColor(0.0,1.0,1,0));
     Graphs::drawArrow(m_simuBall->m_animData.cursors[m_cursor],DataRP::RPColor(1.0,0.0,0,0));
 }

 void RPDrawProcess::drawLines()
 {
    drawbiLine();
    drawContactLines();
    drawLinks();
    drawContour();
 }

 void RPDrawProcess::drawbiLine()
 {
     if(m_biLine.isShow())
     {
        DataRP::Tracks veclist;
        veclist.push_back(m_biLine.line1);
        veclist.push_back(m_biLine.line2);

        Graphs::drawLines(veclist,3,DataRP::RPColor(0.0,1.0,1.0),m_scale,true);
     }
 }

 void RPDrawProcess::drawContactLines()
 {
        if(b_contactLine)
        {

            Graphs::drawLines(m_simuBall->m_acommds[m_contactLine_cursor].DataAppend.contactLines,6,DataRP::RPColor(1.0,1.0,0.0),m_scale,false);
        }
 }

 void RPDrawProcess::drawLinks()
 {
    if(b_showLinks)//m_simuBall->m_animData.buildLinks()
    {
        DataRP::VerticesGrp::iterator it;

        for(it = m_simuBall->m_animData.links.begin(); it!= m_simuBall->m_animData.links.end();it++)
        {
            Graphs::drawLines(*it,2,DataRP::RPColor(1.0,1.0,0.0),true);
        }
    }
    m_simuBall->m_animData.links.clear();
 }

void RPDrawProcess::drawContour()
{

    if(m_simuBall->m_animData.contours.size() > 0)
    {
        DataRP::VerticesGrp::iterator it;

        for(it = m_simuBall->m_animData.contours.begin(); it!= m_simuBall->m_animData.contours.end();it++)
        {
            Graphs::drawLines(*it,2,DataRP::RPColor(1.0,0.0,0.0),true);
        }

    }
}

/****************command******************/
void RPDrawProcess::showBalls(DataRP::AnimCommand &commd)
{
    unsigned length = m_simuBall->m_animData.balls.size();

    if(length < 1)
        return;


    RPDisk ball;
    unsigned index = 0;
    int status = 3;
    unsigned limitRe1 = commd.getIndex();

    m_cursor = m_start;
    m_precursor =  m_start + limitRe1;
    m_simuBall->m_animData.balls[m_cursor].setStatus(DataRP::DR_BALL_CURRENT);
    m_simuBall->m_animData.balls[m_precursor].setStatus(DataRP::DR_BALL_NORMAL);


    ball = m_simuBall->m_animData.balls[0];

    while(status != DataRP::DR_BALL_NORMAL)
    {
        ball = m_simuBall->m_animData.balls[index];
        status = ball.getStatus();
        index++;
    }

    m_radius = ball.getRadius();

    for(unsigned i = 0; i < length; i++)
    {
        m_simuBall->m_animData.balls[i].setStatus(DataRP::DR_BALL_NORMAL);
        m_simuBall->m_animData.balls[i].setRadius(m_radius);
    }

    b_showBalls = true;
    b_cursor = true;
    b_showLinks = false;
}

 void RPDrawProcess::showCursor(DataRP::AnimCommand &commd)
{
    b_cursor = true;
    m_cursor = commd.getIndex() + m_start;
    m_precursor = commd.getValue()+ m_start;

    m_simuBall->m_animData.balls[m_cursor].setStatus(DataRP::DR_BALL_CURRENT);
    m_simuBall->m_animData.balls[m_precursor].setStatus(DataRP::DR_BALL_NORMAL);
}

void RPDrawProcess::deleteBall(DataRP::AnimCommand &commd)
{
    unsigned key = commd.getIndex();
    m_simuBall->m_animData.balls[key].setRadius(0.1);
    m_simuBall->m_animData.balls[key].setStatus(DataRP::DR_BALL_DELETED);
}

void RPDrawProcess::moveBall(DataRP::AnimCommand &commd)
{
    unsigned key = commd.getIndex();
    Vec3D center = commd.DataAppend.center;

    m_simuBall->m_animData.balls[key].setStatus(DataRP::ANP_MOVE_BALL);
    m_simuBall->m_animData.balls[key].setCenter(center);
    m_simuBall->m_animData.balls[key].setRadius(m_radius);
}

void RPDrawProcess::contactLines()
{
    b_contactLine = true;
    m_contactLine_cursor = m_list_Cursor;
}

void RPDrawProcess::showBiLine(DataRP::AnimCommand &commd)
{
    m_biLine = commd.DataAppend.biLine;
    m_biLine.setShow(true);
}

void RPDrawProcess::finishLoop(DataRP::AnimCommand &commd)
{
    clearStatus();
    b_showLinks = true;
    unsigned loopstart,loopend;

    loopstart = commd.getIndex();
    loopend = commd.getValue();
    m_simuBall->m_animData.buildLinks(loopstart,loopend);
    m_start = loopend;
}

void RPDrawProcess::finishAll()
{
    m_timer.stop();
    clearStatus();
    m_list_Cursor = 0;
}

void RPDrawProcess::clearStatus()
{
    m_biLine.setShow(false);
    b_contactLine = false;
    b_cursor = false;
    b_showBalls = false;
    b_showLinks = false;
}

/****************parse commands******************/
void RPDrawProcess::parseCommd()
{

    if(m_simuBall->m_acommds.size() == 0)
    {
        return;
    }

    if(m_list_Cursor == m_simuBall->m_acommds.size())
    {
        reset();
    }

    rp_data->setCurrentCommdPos(m_list_Cursor);
    DataRP::AnimCommand commd;
    commd = m_simuBall->m_acommds[m_list_Cursor];

    b_contactLine = false;
    m_biLine.setShow(false);

    switch(commd.getCommdName())
    {
    case DataRP::ANP_SHOW_BALLS:
       showBalls(commd);
    break;

    case DataRP::ANP_DELETE_BALL:
        deleteBall(commd);
    break;

    case DataRP::ANP_MOVE_BALL:
        moveBall(commd);
    break;

    case DataRP::ANP_UPDATE_CURSOR:
         showCursor(commd);
    break;

    case DataRP::ANP_CONTACT_LINES:
         contactLines();
    break;

     case DataRP::ANP_HELP_LINE:
          showBiLine(commd);
     break;

    case DataRP::ANP_FINISH_LOOP:
         finishLoop(commd);
    break;

    default:
        qDebug()<<"no such command"<<endl;
     break;
    }
    m_list_Cursor++;
    rp_data->refresh();//let gl draw.
}

void RPDrawProcess::gotoCommd(unsigned spos)
{
    if(m_simuBall->m_acommds.size() == 0)
    {
        return;
    }

    if(m_list_Cursor > spos )
    {
        m_list_Cursor = 0;
        m_start = 0;
    }

    while(m_list_Cursor <= spos)
    {
        parseCommd();
    }
}


void RPDrawProcess::timerEvent(QTimerEvent *event)
{
   //qDebug()<<"time id "<<event->timerId();
    Q_ASSERT(event->timerId() > 0);
   if(m_simuBall->isStepOn())
   {
       parseCommd();
       m_simuBall->goOn(false);
   }
}






