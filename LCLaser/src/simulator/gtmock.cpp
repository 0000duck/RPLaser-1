/***************************************************************************
                          gtmock.cpp  -  description
                             -------------------
    begin                : May 20 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "gtmock.h"
#include "graph.h"
#include <QtGlobal>
#include <QTimerEvent>

using namespace SIMURP;

GTMock::GTMock(DataRP::RPData *rpdata,QObject *parent): RPsimu(rpdata,parent),
               m_position(new DataRP::MovePosition()),b_pause(false),
               m_bounding(200),m_overRide(1.0)
{
     connect(this,SIGNAL(printOK()),rpdata,SLOT(simuPrintOK()));
     laserAttr = new DataRP::LaserAttribute();
}

GTMock::~GTMock()
{
    delete m_position;
    delete laserAttr;
}

void GTMock::init(SpeedSet speed)
{
    setPause(false);
    eraseTracks();
    m_simucommds.clear();
    setSpeed(speed);
    m_adjustRate = 1.0f;
    m_overRide = 1.0f;
}

void GTMock::eraseTracks()
{
    rp_data->clearTracks();
}

void GTMock::setPause(bool ispause)
{
    b_pause = ispause;
}

void GTMock::switchLaser(bool bstatus,unsigned short priority)
{
    rp_data->switchLaser(bstatus,priority);
    rp_data->refresh();
}

void GTMock::stopTimer()
{
    if(!rp_data->isPrinting() && m_timer.isActive())
    {
        m_timer.stop();
        qDebug()<<"simu timer is stopped"<<endl;
    }
}

void GTMock::timerEvent(QTimerEvent *event)
{
    if(b_pause)
    {
        rp_data->setLaserPriority(true,1);
        if(laserAttr->canbePush())
        {
            if(rp_data->isLaserOn())
            {
                laserAttr->push(DataRP::LASER_ON);
                switchLaser(false,1);
            }
            else
            {
                laserAttr->push(DataRP::LASER_OFF);
            }
        }

        rp_data->refresh();//for processData.
        return;
    }

    if(laserAttr->canbePop())
    {
        DataRP::LaserStauts lstatus = laserAttr->pop();
        if(lstatus == DataRP::LASER_ON)
        {
            switchLaser(true,1);
        }
        rp_data->setLaserPriority(false,10);
       return;
    }

    //qDebug()<<"time id "<<event->timerId();
    Q_ASSERT(event->timerId() > 0);

    switch (m_currCommd)
    {
        case BuildRP::CT_LnXYZ:
        case BuildRP::CT_LnXY:
            calTracks();
         break;

        case BuildRP::CT_ArcXYP:
        case BuildRP::CT_ArcXY:
            calArc();
        break;

        default:
            checkList();
        break;
    }
}

void GTMock::listernCommends()
{
    //if m_timer = stop;
    m_currCommd = BuildRP::CT_Wait;
    rp_data->m_lastTrackItem.initiate(rp_data->getCompenRadius());

    if(!m_timer.isActive())
        m_timer.start(m_frameInterval,this);
}

void GTMock::checkList()
{
    if(m_simucommds.size() > 0)
    {
         parseCommand();
    }
    rp_data->refresh();//for processData.
}

void GTMock::pushCommands(BuildRP::RPCommd &commd)
{
    m_simucommds.push_back(commd);
}

void GTMock::parseCommand()
{
    BuildRP::RPCommd commd;
    commd = m_simucommds.front();

    switch(commd.getCommand())
    {
        case BuildRP::CT_SetSynVel:
           setsynVel(commd);
        break;

        case BuildRP::CT_LnXYZ:
        case BuildRP::CT_LnXYZG0:
            buffLnXYZ(commd);
        break;

        case BuildRP::CT_LnXY:
            buffLnXY(commd);
        break;

        case BuildRP::CT_ArcXYP:
        case BuildRP::CT_ArcXY:
            buffArcXYP(commd);
        break;

        case BuildRP::CT_BufIOBit:
            BufIOBit(commd);
        break;

        case BuildRP::CT_Finish:
            printFinish(commd);
        break;

          case BuildRP::CT_OVERLAYER:
            synLayerSpeed();
        break;

       default:
            qDebug()<<"no such command"<<endl;
        break;
    }
     m_simucommds.pop_front();//pop up
}

void GTMock::calTracks()
{
    for(int i = 0; i < 3; i++)
    {
        m_position->curr[i] += m_position->step[i];
        m_position->dist[i] += m_position->step[i];
    }

    rp_data->setLaserHeadPosition(Vec3D(m_position->curr[0],m_position->curr[1],m_position->curr[2]));
    rp_data->refreshAnimateTrack(Vec3D(m_position->curr[0],m_position->curr[1],m_position->curr[2]));

    //stop with track end.
    double movingDist = m_position->dist[0]*m_position->dist[0] + m_position->dist[1]*m_position->dist[1] + m_position->dist[2]*m_position->dist[2];
    if(movingDist > m_position->lenth)
    {
        m_currCommd = BuildRP::CT_Wait;
        rp_data->stopTrackAnimate();

        for(int i = 0; i < 3; i++)
        {
            m_position->curr[i] = m_position->target[i];
        }

        buildPath();
    }
    rp_data->refresh();
}

void GTMock::buildPath()
{
    Vec3D tstart = Vec3D( m_position->start[0],m_position->start[1],m_position->start[2]);
    Vec3D tend = Vec3D( m_position->target[0],m_position->target[1],m_position->target[2]);

    if(rp_data->isLaserOn())
    {
        if(tstart.getX() != tend.getX() || tstart.getY() != tend.getY())
        {
            Line line = Line(tstart,tend);
            rp_data->buildTracks(line);
        }
    }
}

void GTMock::timeConsuming()
{

}


/***********GT_Commands**************************/

int GTMock::GT_ClrSts(unsigned short axis)
{
    Q_ASSERT(axis > 4);
    return 0;
}

void GTMock::setSpeed(SpeedSet speed, int type)
{
    switch(type)
    {
        case 1:
           m_vel = speed.getOutlineVel();
        break;

        case 2:
            m_vel = speed.getFillingVel();
        break;

        default:
           m_vel = speed.getMoveVel();
        break;
    }


}

void GTMock::adjustSpeed(double srate)
{
    m_adjustRate = srate;
}

int GTMock::GT_Override(double value)
{
    m_overRide = value;
    return 0;
}

int GTMock::GT_SetHSIOOpt(unsigned short value)
{
    if(value ==1)
        switchLaser(true);
    else
        switchLaser(false);

    return 0;
}

int GTMock::GT_GetHSIOOpt(unsigned short *exopt)
{
    if(rp_data->isLaserOn())
    {
        *exopt = 1;
    }
    else
    {
        *exopt = 0;
    }
   return 0;
}

int GTMock::GT_ZeroPos(unsigned short axisNum)
{
    if(axisNum > 3)
        return -1;
    axisNum--;
    m_position->curr[axisNum] = 0;
    moveLaserHeader();
    return 0;
}

void GTMock::GT_LnXYZ(double sx,double sy,double sz)
{
    m_position->curr[0] = sx;
    m_position->curr[1] = sy;
    m_position->curr[2] = sz;
    moveLaserHeader();
}

void GTMock::GT_SetAtlPos(unsigned short AxisNum, double pos)
{
    int axis = AxisNum-1;
    m_position->curr[axis] = pos;
}

void GTMock::GT_JumpXYZ(double sx,double sy,double sz)
{
    m_position->curr[0] += sx;
    m_position->curr[1] += sy;
    m_position->curr[2] += sz;
    moveLaserHeader();
}

void GTMock::moveLaserHeader()
{
//   can move negative.
//    if(m_position->curr[0] > m_bounding)
//        m_position->curr[0] = m_bounding;
//    if(m_position->curr[0] < -m_bounding)
//        m_position->curr[0] = -m_bounding;
//    if(m_position->curr[1] > m_bounding)
//        m_position->curr[1] = m_bounding;
//    if(m_position->curr[1] < -m_bounding)
//        m_position->curr[1] = -m_bounding;
//    if(m_position->curr[2] > m_bounding)
//        m_position->curr[2] = m_bounding;

    //    if(m_position->curr[2] < 0)
    //        m_position->curr[2] = 0;

    rp_data->setLaserHeadPosition(Vec3D(m_position->curr[0],m_position->curr[1],m_position->curr[2]));
    rp_data->refresh();
}

int GTMock::terminatePrint()
{
    GT_SetHSIOOpt(0);
    m_simucommds.clear();
    stopTimer();
    qDebug()<<"manual interrupt"<<endl;
    return 0;
}

int GTMock::GT_GetCrdSts(unsigned short *coord_status)
{
    if(m_timer.isActive())
    {
         *coord_status  = 0;
    }
    else//stop
    {
         *coord_status  = 0x2001;
    }
    return 0;
}

int GTMock::GT_GetPrfPnt(double *pnt)
{
    for(int i = 0; i < 3; i++)
    {
        *pnt = m_position->curr[i];
        pnt++;
    }
    return 0;
}

void GTMock::goHome(unsigned short axis)
{
    m_position->curr[axis-1] = 0;
    moveLaserHeader();
}

/***********buff print**************************/
void GTMock::buffLnXYZ(BuildRP::RPCommd &commd)
{
    double len[3];
    int i;
    double vel = m_vel * m_adjustRate *m_overRide;
    double sx = 0, sy = 0, sz = 0;
    commd.getValue(sx,sy,sz);

    Vec3D linevec = Vec3D(sx,sy,sz);
    m_position->target[0] = linevec.getX();
    m_position->target[1] = linevec.getY();
    m_position->target[2] = linevec.getZ();

    Vec3D beginPos = Vec3D(m_position->curr[0],m_position->curr[1],m_position->curr[2]);

    linevec -= beginPos;
    m_position->lenth = linevec.Length2();

    //direction.
    linevec.Normalize();
    len[0] = linevec.getX();
    len[1] = linevec.getY();
    len[2] = linevec.getZ();

    for(i = 0; i < 3; i++)
    {
       m_position->dist[i]  = 0;
       m_position->step[i]  = vel*m_frameInterval*len[i]/1000;// mm/s * ms/F
       m_position->start[i] = m_position->curr[i];//start.
    }

    rp_data->setAnimateTrack(Vec3D(m_position->start[0],m_position->start[1],m_position->start[2]));
    m_currCommd = BuildRP::CT_LnXYZ;
}

void GTMock::setsynVel(BuildRP::RPCommd &commd)
{
     double tv;
     commd.getValue(tv);
     m_vel = tv;
}

void GTMock::BufIOBit(BuildRP::RPCommd &commd)
{
    short bit_status;
    double tx,ty;

    commd.getValue(tx,ty);
    bit_status = (short)ty;

    if(bit_status == 1)
      switchLaser(true);
    else
      switchLaser(false);

     m_currCommd = BuildRP::CT_BufIOBit;
}

void GTMock::printFinish(BuildRP::RPCommd &commd)
{
    double sx = 0, sy = 0, sz = 0;
    commd.getValue(sx,sy,sz);
    GT_LnXYZ(sx,sy,sz);

    m_timer.stop();
    emit printOK();
}

void GTMock::synLayerSpeed()
{
    rp_data->increaseSimuLevel();
    rp_data->exchangeDrawTraks();
}

void GTMock::buffLnXY(BuildRP::RPCommd &commd)
{
    double len[3];
    int i;
    double vel = m_vel * m_adjustRate *m_overRide;
    double sx = 0, sy = 0;
    double sz = m_position->curr[2];
    commd.getValue(sx,sy);

    Vec3D linevec = Vec3D(sx,sy,sz);
    linevec.setX(linevec.getX());
    linevec.setY(linevec.getY());

    m_position->target[0] = linevec.getX();
    m_position->target[1] = linevec.getY();
    m_position->target[2] = linevec.getZ();

    Vec3D beginPos = Vec3D(m_position->curr[0],m_position->curr[1],m_position->curr[2]);

    linevec -= beginPos;
    m_position->lenth = linevec.Length2();

    if(linevec == Vec3D(0.0,0.0,0.0))
    {
        qDebug()<<"cannot animation"<<endl;
        return;
    }

    //direction.
    linevec.Normalize();
    len[0] = linevec.getX();
    len[1] = linevec.getY();
    len[2] = linevec.getZ();

    for(i = 0; i < 3; i++)
    {
       m_position->dist[i]  = 0;
        // mm/ms * 10ms/F = mm/F
       m_position->step[i]  = vel*m_frameInterval*len[i]/500;// mm/s * ms/F
       m_position->start[i] = m_position->curr[i];//start.
    }

    rp_data->setAnimateTrack(Vec3D(m_position->start[0],m_position->start[1],m_position->start[2]));
    m_currCommd = BuildRP::CT_LnXY;
}

void GTMock::buffArcXYP(BuildRP::RPCommd &commd)
{
    double vel = m_vel * m_adjustRate *m_overRide;
    Vec3D beginPos = Vec3D(m_position->curr[0],m_position->curr[1],m_position->curr[2]);
    for(int i = 0; i < 3; i++)
    {
       m_position->start[i] = m_position->curr[i];//start.
    }

    double currentZ = m_position->curr[2];

    double sx,sy,sz,sa,sb,sc;
    double sradius,radian,cosine;
    Vec3D endPos,center;
    short direct;
    double interval,subRadian;

    int arctype = commd.getCommand();
    commd.getValue(sx,sy,sz,sa,sb,sc);

    if(arctype == BuildRP::CT_ArcXYP)
    {
        endPos = Vec3D(sx,sy,currentZ);
        sradius = sz;
        direct = (short)sa;
        center = Vec3D(sb,sc,currentZ);

        if(direct !=1 && direct !=-1)
        {
          qDebug()<<"gtmock no direction"<<endl;
          return;
        }
    }
    else if (arctype == BuildRP::CT_ArcXY)
    {
        center = Vec3D(sx,sy,currentZ);
        radian = sz;
        sradius = sa;
        endPos = Vec3D(sb,sc,currentZ);
     }

    endPos.setX(endPos.getX());
    endPos.setY(endPos.getY());
    center.setX(center.getX());
    center.setY(center.getY());
    sradius = sradius;

    m_position->target[0] = endPos.getX();
    m_position->target[1] = endPos.getY();
    m_position->target[2] = endPos.getZ();


    if(arctype == BuildRP::CT_ArcXYP)
    {
        Vec3D vstart = beginPos - center;
        Vec3D vend = endPos - center;
        vstart.Normalize();
        vend.Normalize();

       if(direct==1)
       {
          cosine = vstart.Dot(vend);
       }
       else
       {
          cosine = vend.Dot(vstart);
       }
       radian = acos(cosine); //(0~PI)

       if(sradius < 0)//major Arc
       {
           radian = 2*M_PI - radian;
           sradius = -sradius;
       }

       if(direct== -1)
          radian = -radian;
    }

    //arcLen = M_PI*sradius*radian;steps = arcLen/interval;subRadian = radian / steps;
    interval = vel * m_frameInterval/500;// mm/s * ms/F
    subRadian = interval/(M_PI*sradius);

    m_position->curr[0] = 0;
    m_position->lenth = radian;
    if( m_position->lenth < 0)
        m_position->step[0] = -subRadian;
    else
        m_position->step[0] = subRadian;

    rp_data->setAnimateArc(beginPos,center,sradius,radian);
    m_currCommd = arctype;
    m_cnt = 0;
}

void GTMock::calArc()
{
    m_cnt++;
    Vec3D pos;
    double diff;
    m_position->curr[0] += m_position->step[0];
    rp_data->refreshAnimateArc(m_position->curr[0]);
    rp_data->getcurArcPos(pos);
    rp_data->setLaserHeadPosition(pos);
    //stop with track end.
    diff = m_position->lenth -m_position->curr[0];
    if(m_position->lenth < 0)
               diff = -diff;

    if(diff < 0)
    {
        m_currCommd = BuildRP::CT_Wait;
        rp_data->stopArcAnimate();

        for(int i = 0; i < 3; i++)
        {
            m_position->curr[i] = m_position->target[i];
        }
        Vec3D endPos = Vec3D(m_position->target[0],m_position->target[1],m_position->target[2]);
        rp_data->buildArc(endPos);
    }
    rp_data->refresh();
}
