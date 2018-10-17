/***************************************************************************
                          RPControl.cpp  -  description
                             -------------------
    begin                : Apr. 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QFile>
#include <QDir>
#include "control.h"
#include "ges.h"
#include <QDebug>
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <QMessageBox>
#include <QRegExp>
#include "buffthread.h"
#include "tabcontrol.h"
#include "threadlog.h"
#include "initialization.h"

using namespace BuildRP;

bool RPControl::b_simu = true;

RPControl::RPControl(QObject *parent) : ControlInterface(parent),
    m_frequency(10),m_dutyratio(10),m_voltage(10),
    m_position(Vec3D(0,0,0)),
    m_tryposition(Vec3D(0,0,0)),
    b_pause(false),
    b_runtimeSet(false),
    b_printRun(false),
    m_homeAxis(7),
    m_laserPowerMode(LPMODE_PWM)
{
    m_rpp = RPConfig::content.getSpeedrpp();
    m_pulseEqu = RPConfig::content.getPulseEquivalent();
    laserAttr = new DataRP::LaserAttribute();
    RPConfig::content.getOrigin(m_origin[0],m_origin[1],m_origin[2]);
    RPConfig::content.getSeekOrigin(m_seekOrigin[0],m_seekOrigin[1],m_seekOrigin[2]);
}

RPControl::~RPControl()
{
    if(rp_simu != NULL)
        delete rp_simu;

    delete laserAttr;
}
/************update***********************/

void RPControl::refresh()
{
    emit updateDisplay();
}

/***********call back************************/

void RPControl::CommandHandle(char *command,short error)
{

    if(isSimulate())
    {
       return;
    }

    unsigned short status;
    QString strErr;
    QString scommand = QString(QLatin1String(command));
    switch(error)
    {
    case -1:
        strErr = QString("\a\nCommunication Error !");
        qDebug()<<scommand<<" "<<strErr;
        break;
    case 0:
        break;
    case 1:
        strErr = QString("\a\nCommand Error !");
        GT_GetCmdSts(&status);
        if(status == 32768) // buff is full.
        {
            return;
        }
        strErr.append(QString::number(status,10));
        qDebug()<<scommand<<" "<<strErr;
        break;
    case 2:
        strErr = QString("\a\nRadius or chord is 0 !");
        qDebug()<<scommand<<" "<<strErr;
        break;
    case 3:
        strErr = QString("\a\nLength is 0 or overflow !");
        break;
    case 4:
        strErr = QString("\a\nVelocity or acceleration is less then 0 !");
        qDebug()<<scommand<<" "<<strErr;
        break;
    case 5:
        strErr = QString("\a\nChord is greater than diameter !");
        qDebug()<<scommand<<" "<<strErr;
        break;
    case 7:
        strErr = QString("\a\nParameter error !");
        qDebug()<<scommand<<" "<<strErr;
        break;
    default:
        strErr = "the error code is: ";
        QString scode = QString::number(error,10);
        strErr.append(scode);
         qDebug()<<scommand<<" "<<strErr;
        break;
    }
}

/*********** interface ************************/
void RPControl::singleBuffOn()
{
    //GT_HookCommand(CommandHandle);
    setLaser();
    double sx,sy,sz;
    sx = singleLenghtRadius/2;  //30000
    sy = 0;
    sz = 0;
    m_tryposition.setX(sx); //30000
    m_tryposition.setY(sy); //30000
    m_tryposition.setZ(sz); //30000
    laserAttr->clear();
    moveXYZfromZero(1000,0,0);

    GT_StrtList();
    GT_MvXYZ(0, 0, 0, velConvert(m_speed.getOutlineVel()), accConvert(m_speed.getOutlineAcc()));//10,0.5
    GT_LnXYZG0(-m_tryposition.getX(),m_tryposition.getY(),m_tryposition.getZ());
    for(unsigned i = 0; i <singleMaxLayer; i++)
    {
        if(singleTrackType == 0)
            drawLineZ();
        else if(singleTrackType == 1)
            drawLine();
        else if(singleTrackType == 2)
            drawArc();
    }

    GT_EndList();
    GT_StrtMtn();
}

void RPControl::singleBuffOff()
{
    //GT_HookCommand(CommandHandle);
    double prf_pos[4];

    GT_StpMtn();   //stop
    if(laserAttr->canbePush())
    {
        if(getLaserStatus())
        {
            laserAttr->push(DataRP::LASER_ON);
            switchLaser(false);
        }
        else
        {
            laserAttr->push(DataRP::LASER_OFF);
        }



#ifdef WIN32
           Sleep(400);
#else
           usleep(400000);
#endif

        GT_GetPrfPnt(prf_pos);
        m_position.setZ(prf_pos[2]);
    }
}

void RPControl::singleBuffContinue()
{
    //GT_HookCommand(CommandHandle);
    if(laserAttr->canbePop())
    {
        long pos;
        pos = m_position.getZ();
        GT_SetAtlPos(3,pos);
        GT_RestoreMtn();

        DataRP::LaserStauts lstatus = laserAttr->pop();
        if(lstatus == DataRP::LASER_ON)
        {
            switchLaser(true);
        }
    }
}

void RPControl::SingleClearBuff()
{
    singleBuffOff();
    laserAttr->pop();
    GT_StrtList();
    GT_EndList();
}

void RPControl::drawLine()
{
    drawLineZ();
    GT_LnXYZG0(m_tryposition.getX(),m_tryposition.getY(),m_tryposition.getZ());
    m_tryposition.setX(-m_tryposition.getX());
}

void RPControl::drawLineZ()
{
    //GT_HookCommand(CommandHandle);
    GT_BufIOBit(1,1);
    GT_LnXYZ(m_tryposition.getX(),m_tryposition.getY(),m_tryposition.getZ());//5000
    GT_BufIOBit(1,0);

    m_tryposition.setZ(m_tryposition.getZ()+ singledeltaZ);
    GT_LnXYZG0(m_tryposition.getX(),m_tryposition.getY(),m_tryposition.getZ());
    m_tryposition.setX(-m_tryposition.getX());
}

void RPControl::drawArc()
{
    //GT_HookCommand(CommandHandle);

    GT_LnXYZG0(m_tryposition.getX(),m_tryposition.getY(),m_tryposition.getZ());//5000
    GT_BufIOBit(1,1);
    GT_ArcXY(0,0,360);//10000
    GT_BufIOBit(1,0);
    m_tryposition.setZ(m_tryposition.getZ()+ singledeltaZ);
}


/************setting***********************/
void RPControl::setMoveSpeed()
{
    //GT_HookCommand(CommandHandle);
    GT_SetSynVel(velConvert(m_speed.getMoveVel()));
    GT_SetSynAcc(accConvert(m_speed.getMoveAcc()));
    rp_simu->setSpeed(m_speed);
}

void RPControl::setFillingSpeed()
{
    //GT_HookCommand(CommandHandle);
    GT_SetSynVel(velConvert(m_speed.getFillingVel()));
    GT_SetSynAcc(accConvert(m_speed.getFillingAcc()));
    rp_simu->setSpeed(m_speed,1);
}

void RPControl::setOutlineSpeed()
{
    //GT_HookCommand(CommandHandle);
    GT_SetSynVel(velConvert(m_speed.getOutlineVel()));
    GT_SetSynAcc(accConvert(m_speed.getOutlineAcc()));
    rp_simu->setSpeed(m_speed,2);
}

void RPControl::setData(DataRP::RPData *pdata)
{
    if(pdata!=NULL)
    {
        rp_data = pdata;
        rp_simu = new SIMURP::GTMock(rp_data);
        rp_data->setSimu(rp_simu);
        rp_data->setControl(this);
    }
}

void RPControl::setLaser()
{
    laserWithPowerMode();

    if(laser_offDelay != 0 && laser_onDelay != 0)
         GT_SetLevelDelay(laser_offDelay,laser_onDelay);

}

void RPControl::laserWithPowerMode()
{
    //GT_HookCommand(CommandHandle);
    if(m_laserPowerMode ==  LPMODE_PWM )
    {
        GT_SetLaserMode(LASER_CHANNEL,1);
        GT_LaserPowerMode(0,100,0,LASER_CHANNEL);
        GT_LaserOutFrq(m_frequency,LASER_CHANNEL);
        GT_LaserPrfCmd(m_dutyratio,LASER_CHANNEL);
    }
    else if(m_laserPowerMode ==  LPMODE_DA)
    {
        GT_LaserPowerMode(2,24,0,LASER_CHANNEL);
        GT_LaserPrfCmd(m_voltage,LASER_CHANNEL);

    }

}

void RPControl::setFrequency(double frequency)
{
    //GT_HookCommand(CommandHandle);
    m_frequency = frequency;
    GT_LaserOutFrq(m_frequency,LASER_CHANNEL);
}

void RPControl::setDutyratio(double dutyratio)
{
    m_dutyratio = dutyratio;
}

void RPControl::setVoltage(double voltage)
{
    m_voltage = voltage;
}

void RPControl::setMoveVelValue(double sv)
{
    m_speed.setMoveVel(sv);
}

void RPControl::setMoveAccValue(double sv)
{
    m_speed.setMoveAcc(sv);
}

void RPControl::setOutlineVelValue(double sv)
{
    m_speed.setOutlineVel(sv);
}

void RPControl::setFillingVelValue(double sv)
{
    m_speed.setFillingVel(sv);
}

void RPControl::setOutlineFillingAccValue(double sv)
{
    m_speed.setOutlineAcc(sv);
}

void RPControl::switchSIMU(bool isOn)
{
    b_simu = isOn;
}

void RPControl::clearCommds()
{
    m_commds.clear();
}

void RPControl::setSingleHeight(int smaxHeight)
{
    singleMaxLayer = (unsigned)smaxHeight;
}

void RPControl::setSingleLenghtRadius(double svalue)
{
    singleLenghtRadius = svalue*m_pulseEqu;
}

void RPControl::SetSingleDeltaZ(double sdZ)
{
    singledeltaZ = sdZ*m_pulseEqu;
}

void RPControl::setSingleTrack(int trackType)
{
    singleTrackType = trackType;
}

void RPControl::setLaserOnDelay(double son)
{
    laser_onDelay = son;
}

void RPControl::setLaserOffDelay(double soff)
{
    laser_offDelay = soff;
}

void RPControl::setParaminRuntime()
{
    if(b_printRun)
    {
        b_runtimeSet = true;
    }
    else
    {
        if(m_laserPowerMode == LPMODE_PWM)
        {
            GT_LaserPrfCmd(m_dutyratio,LASER_CHANNEL);
        }
        else if(m_laserPowerMode == LPMODE_DA)
        {
            GT_LaserPrfCmd(m_voltage,LASER_CHANNEL);
        }

        GT_Override(m_override);
        qDebug()<<"set in time"<<m_override<<endl;
    #ifdef WIN32
    Sleep(800);
    #else
    usleep(800000);
    #endif
    }
}

double RPControl::mm2pulse(double smm)
{
    return smm*m_pulseEqu;
}

double RPControl::pulse2mm(double pulse)
{
    return pulse/m_pulseEqu;
}

double RPControl::velConvert(double vel)
{
     return (vel*m_pulseEqu)/1000;
}

double RPControl::accConvert(double acc)
{
    return (acc*m_pulseEqu)/1000000;
}

double RPControl::getPrintZ()
{
    if(b_simu)
    {
        return rp_data->getCurSimZ();
    }

     //GT_HookCommand(CommandHandle);
    double prf_pos[4];
    GT_GetPrfPnt(prf_pos);
    QString strz;
    strz.setNum(prf_pos[2]);
    //qDebug()<<"pos z is"<<strz<<endl;
    return pulse2mm(prf_pos[2]);
 }

void RPControl::getCurrentPosition(Vec4D &pos)
{
    double prf_pos[4];
    if(b_simu)
    {
         rp_simu-> GT_GetPrfPnt(prf_pos);
    }
    else
    {
        //GT_HookCommand(CommandHandle);
        GT_GetPrfPnt(prf_pos);
        for(int i = 0; i < 4; i++)
            prf_pos[i] = pulse2mm(prf_pos[i]);
    }
   pos.setValue(prf_pos);
   pos.setValue(1);
}

/************judge***********************/

bool RPControl::isSimulate()
{
     if(b_simu)
        return true;

     return false;
}

bool RPControl::isPrintRun()
{
    return b_printRun;
}
/************trun on***********************/

bool RPControl::connectMachine()
{
    if(isSimulate())
    {
        qDebug()<<" connect virtual machine "<<endl;
        return true;
    }

    unsigned short status;
    //GT_HookCommand(CommandHandle);

    if(GT_Open() != 0)
    {
       GT_Close();
       qDebug()<<" cannot connect machine "<<endl;
       return false;
    }
    else
    {
       GT_Reset();
       GT_LmtSns(0xFF);//limit set.
       GT_SetStrtVel(0);
       for(int axis = 1; axis < 5; axis++)
       {
           GT_CtrlMode(axis,1);
           GT_GetSts(axis,&status);
           if(status&0x0200)//1 enalbe
           {
                GT_AxisOff(axis);
           }
           else
           {
               continue;
           }

#ifdef WIN32
           Sleep(200);
#else
           usleep(200000);
#endif
           GT_ClrSts(axis);
       }
        emit updateLaserStatus();
    }
   return true;
}

void RPControl::turnOnLaser()
{
   //GT_HookCommand(CommandHandle);
   setLaser();
   GT_SetHSIOOpt(1);
   //simu
   rp_simu->GT_SetHSIOOpt(1);
   emit updateLaserStatus();
}

bool RPControl::enableAxis()
{
    bool bresult = true;

    if(isSimulate())
    {
        return bresult;
    }

    short rtn;
    unsigned short status;
    //GT_HookCommand(CommandHandle);
    //every axis enable.
    for(unsigned short axis = 1; axis < 5; axis++)
    {
        rtn = GT_GetSts(axis,&status);
        rtn += GT_AlarmOff(axis);
        rtn += GT_EncOff(axis);
        rtn += GT_LmtsOn(axis);
        rtn += GT_ClrSts(axis);

        if(!(status&0x0200))//0 disable
        {
            rtn += GT_AxisOn(axis);
        }
        else
        {
            continue;
        }
#ifdef WIN32
           Sleep(200);
#else
           usleep(200000);
#endif
        rtn += GT_StepDir(axis);//GT_StepPulse
        rtn += GT_ZeroPos(axis);//go to zero.


        qDebug()<<"enabl axis :"<<axis<<endl;
        //rtn += GT_DrvRst(axis);

        if(rtn)
        {
            GT_GetCmdSts(&status);
            QString strErr = "cannot turn on Axis : ";
            QString scode = QString::number(axis,10);
            strErr.append(scode);
            strErr.append(" code is ");
            scode =  QString::number(status,10);
            strErr.append(scode);
            QMessageBox::critical(0,QObject::tr("enable Axis error"),strErr);
            bresult = false;
        }
    }

    if(rtn ==0)
    {
            getLimitStatus();
    }

    return bresult;
}


/************trun off***********************/

void RPControl::disConnectMachine()
{
    GT_Close();
}

void RPControl::turnOffLaser()
{
   //GT_HookCommand(CommandHandle);
   GT_SetHSIOOpt(0);
   rp_simu->GT_SetHSIOOpt(0);
   emit updateLaserStatus();
}

void RPControl::switchLaser(bool on)
{
    if(on)
    {
        GT_SetHSIOOpt(1);
    }
    else
    {
        GT_SetHSIOOpt(0);
    }
}

bool RPControl::disableAxis()
{
    short rtn;
    bool bresult = true;
    unsigned short status;
    //GT_HookCommand(CommandHandle);
    for(int axis = 1; axis < 5; axis++)
    {
        rtn = GT_GetSts(axis,&status);
        if(status&0x9)//1 enalbe
        {
             rtn += GT_AxisOff(axis);
        }
        else
        {
            continue;
        }

#ifdef WIN32
           Sleep(200);
#else
           usleep(200000);
#endif
        rtn = GT_ClrSts(axis);

        if(rtn)
        {
            QString strErr = "cannot turn off Axis is: ";
            QString scode = QString::number(axis,10);
            strErr.append(scode);
            QMessageBox::critical(0,QObject::tr("enable Axis error"),strErr);
            bresult = false;
        }
    }

    return bresult;
}

/************status**************************/

void RPControl::clearLimit()
{
    for(unsigned short axis = 1; axis < 5; axis++)
    {
        GT_ClrSts(axis);
        //qDebug()<<" ClrSts : "<<axis<<endl;
    }
}

void RPControl::getLimitStatus()
{
    //GT_HookCommand(CommandHandle);
    unsigned short limit = 0;
    GT_GetLmtSwt(&limit);
    //qDebug()<<"limit  "<<limit<<endl;
    if((limit&0x3F) != 0x3F )
    {
        QString strErr = "";
        QMessageBox::critical(0,QObject::tr("one limit invalid"),strErr);
    }
}

void RPControl::getStatus(VStatus& status)
{
    //GT_HookCommand(CommandHandle);
    unsigned short stat,i;

    for(i=0;i<3;i++)
    {
        GT_GetSts(i+1,&stat);

        if(stat & 0x0200)
        {
           status[i][0] = true;
        }
        else
        {
           status[i][0] = false;
        }

        if(stat & 0x0020)
        {
           status[i][1] = true;
        }
        else
        {
           status[i][1]= false;
        }

        if(stat & 0x0040)
        {
           status[i][2] = true;
        }
        else
        {
           status[i][2]= false;
        }
   }
}

bool RPControl::getLaserStatus()
{
    unsigned short status = 0;
    if(isSimulate())
    {
        rp_simu->GT_GetHSIOOpt(&status);
        if(status & 0x1 )
        {
            return true;
        }
         return false;
    }

    //GT_HookCommand(CommandHandle);

    GT_GetHSIOOpt(&status);

    if(status & 0x1 )
    {
        return true;
    }
    return false;
}

void RPControl::setOveride(double sr)
{
    m_override  = sr;
}

void RPControl::setHomeAxis(unsigned short haxis)
{
    if(haxis >7)
        return;
    m_homeAxis = haxis;
}

/************move****************************/
void RPControl::move2Start()
{
    Vec3D firstPos = rp_data->getFirstPosition();
    firstPos.setZ(firstPos.getZ()+30);
    double start_pos[4];
    start_pos[0] = mm2pulse(firstPos.getX());
    start_pos[1] = mm2pulse(firstPos.getY());
    start_pos[2] = mm2pulse(firstPos.getZ());
    start_pos[3] = 0;
    moveXYZfromZero(start_pos[0],start_pos[1],start_pos[2]);
}

void RPControl::move2MeasurePos(Vec3D spos)
{
    double start_pos[4];
    start_pos[0] = mm2pulse(spos.getX());
    start_pos[1] = mm2pulse(spos.getY());
    start_pos[2] = mm2pulse(spos.getZ()+8);
    moveXYZfromZero(start_pos[0],start_pos[1],start_pos[2]+5e4);
    moveXYZfromZero(start_pos[0],start_pos[1],start_pos[2]);
}

void RPControl::resetZero()
{
    if(b_printRun)
        return;

    unsigned short rtn = 0;
    QString str;
    //clearCommds();
    for(unsigned short axes = 1; axes < 5;axes++)
    {
        rtn += GT_ZeroPos(axes);
        rp_simu->GT_ZeroPos(axes);

        if(rtn)
        {
            str = QString::number(rtn,10);
            QMessageBox::critical(0,QObject::tr("GT_ZeroPos error"), str);
            return;
        }
    }
    QString pos = "0,   0,   0";
    emit showInfo(pos);
}

void RPControl::moveOffset()
{
    double prf_pos[4];
    GT_GetPrfPnt(prf_pos);

    if(prf_pos[0] == 0 && prf_pos[1] == 0 && prf_pos[2] == 0)
        moveXYZ(1000,1000,0);
}

bool RPControl::shouldMoveOffset(const double (&pos)[4])
{
    double prf_pos[4];
    GT_GetPrfPnt(prf_pos);

    if(prf_pos[0] == pos[0] && prf_pos[1] == pos[1] && prf_pos[2] == pos[2])
    {
        moveXYZ(1000,1000,1000);
        return true;
    }
    return false;
}

//!input pulse.
void RPControl::moveXYZfromZero(unsigned int saxis,double srange)
{
    if(b_printRun)
        return;

    if(saxis < 1 || saxis > 4)
        return;

    unsigned int axis = saxis - 1;

    short rtn;
    unsigned short status = 0;
    QString str;
    double prf_pos[4];
    GT_GetPrfPnt(prf_pos);

    emit showInfo("");
    if(prf_pos[axis] != srange)
    {
        prf_pos[axis] = srange;
        setMoveSpeed();
        rtn = GT_LnXYZ(prf_pos[0],prf_pos[1],prf_pos[2]);

        while(!(status&0x01))
        {
            if(isSimulate())
            {
                break;
            }

            rtn = GT_GetCrdSts(&status);

            if (rtn)
            {
                str = QString::number(rtn,10);
                QMessageBox::critical(0,QObject::tr("get crd error"), str);
                return;
            }
        }
    }
    rp_simu->GT_LnXYZ(prf_pos[0]/m_pulseEqu,prf_pos[1]/m_pulseEqu,prf_pos[2]/m_pulseEqu);
    Vec4D pos;
    str = "";
    getCurrentPosition(pos);
    for(int i = 0; i < 3; i++)
    {
         str.append(QString::number(pos.getValue(i),4,2));
         if(i != 2)
            str.append(",  ");
    }
    emit showInfo(str);
}

//!input pulse.
void RPControl::moveXYZfromZero(double sx, double sy, double sz)
{
    if(b_printRun)
        return;

    short rtn;
    unsigned short status = 0;
    QString str;
    double prf_pos[4];
    GT_GetPrfPnt(prf_pos);

    emit showInfo("");
    if(!(prf_pos[0] == sx && prf_pos[1] == sy && prf_pos[2] == sz))
    {
        setMoveSpeed();
        rtn = GT_LnXYZ(sx,sy,sz);

        while(!(status&0x01))
        {
            if(isSimulate())
            {
                break;
            }

            rtn = GT_GetCrdSts(&status);

            if (rtn)
            {
                str = QString::number(rtn,10);
                QMessageBox::critical(0,QObject::tr("get crd error"), str);
                return;
            }
        }
    }

    rp_simu->GT_LnXYZ(sx/m_pulseEqu,sy/m_pulseEqu,sz/m_pulseEqu);
    Vec4D pos;
    str = "";
    getCurrentPosition(pos);
    for(int i = 0; i < 3; i++)
    {
         str.append(QString::number(pos.getValue(i),4,2));
         if(i != 2)
            str.append(",  ");
    }
    emit showInfo(str);
}

//! input pulse.
void RPControl::moveXYZ(double sx, double sy, double sz)
{
    short rtn;
    unsigned short status = 0;
    QString str;

    setMoveSpeed();
    double prf_pos[4];
    rtn = GT_GetPrfPnt(prf_pos);
    rtn += GT_LnXYZ(prf_pos[0]+sx,prf_pos[1]+sy,prf_pos[2]+sz);

    if(isSimulate())
    {
        rtn = 0;
    }

    if (rtn)
    {
        str = QString::number(rtn,10);
        QMessageBox::critical(0,QObject::tr("move error"), str);
        return;
    }

    emit showInfo("");
    while(!(status&0x01))
    {
        if(isSimulate())
        {
            break;
        }

        rtn = GT_GetCrdSts(&status);

        if (rtn)
        {
            str = QString::number(rtn,10);
            QMessageBox::critical(0,QObject::tr("get crd error"), str);
            return;
        }
    }

    rp_simu->GT_JumpXYZ(sx/m_pulseEqu,sy/m_pulseEqu,sz/m_pulseEqu);
    QString info;
    Vec4D pos;
    getCurrentPosition(pos);
    for(int i = 0; i < 3; i++)
    {
     info.append(QString::number(pos.getValue(i),4,2));
     if(i != 2)
        info.append(",  ");
    }
    emit showInfo(info);
}

//! input pulse.
void RPControl::threadMoveXYZ(double sx, double sy, double sz,double vel,double acc)
{
    if(b_printRun)
        return;

    short rtn;
    unsigned short status = 0;
    QString str;

    rtn = GT_SetSynVel(vel);
    rtn += GT_SetSynAcc(acc);
    rtn += GT_LnXYZ(sx,sy,sz);

    if(isSimulate())
    {
        rtn = 0;
    }

    if (rtn)
    {
        str = QString::number(rtn,10);
        QMessageBox::critical(0,QObject::tr("move error"), str);
        return;
    }

    while(!(status&0x01))
    {
        if(isSimulate())
        {
            break;
        }

        rtn = GT_GetCrdSts(&status);

        if (rtn)
        {
            str = QString::number(rtn,10);
            QMessageBox::critical(0,QObject::tr("get crd error"), str);
            return;
        }
    }
}


/************data processing***************************/

//!get commds and parameters form data.
void RPControl::getCommds()
{
    qDebug()<<"          ++++begin pushing commds++++"<<endl;
    RPprintItem track;
    Line tline;
    int runtype = 0;
    RPCommd comd;

    comd.setCommand(CT_BuffOn);
    m_mutex.lock();
    m_commds.push_back(comd);
    m_mutex.unlock();
    //loop
    rp_data->extractData(extractOutlines,extractPaths,this,track,runtype,tline);
    qDebug()<<"command loop over  "<<m_commds.size()<<endl;
}

//!extract outlines.
void  RPControl::extractOutlines(RPControl* rpconntrol,RPprintItem &item,int &runtype)
{
    rpconntrol->m_mutex.lock();
    rpconntrol->dealOutline(item,runtype);
    rpconntrol->m_mutex.unlock();
}

//!extract paths.
void RPControl::extractPaths(RPControl* rpconntrol,Line &line)
{
    rpconntrol->m_mutex.lock();
    rpconntrol->dealPath(line);
    rpconntrol->m_mutex.unlock();
}

//!deal extracted outlines.
void RPControl::dealOutline(RPprintItem &item,int &runtype)
{
    RPCommd comd;

    switch(runtype)
    {
     case DataRP::TRAVEL_FINISH:
        comd.setCommand(CT_BufIOBit);
        comd.setStatusValue(1,0);
        m_commds.push_back(comd);

        comd.clear();
        comd.setCommand(CT_SetSynVel);
        comd.setsynVel(m_speed.getFillingVel());
        m_commds.push_back(comd);

        comd.clear();
        comd.setCommand(CT_LnXYZG0);
        comd.setLnXYZ(item.getEndPos());
        m_commds.push_back(comd);

        comd.clear();
        comd.setCommand(CT_Finish);
        comd.setLnXYZ(0,0,(rp_data->getMaxLayer()+1.0));
        m_commds.push_back(comd);
     break;

     case DataRP::TRAVEL_LAYEROVER:
        comd.setCommand(CT_OVERLAYER);
        comd.setStatusValue(0,0);
        m_commds.push_back(comd);

     break;

     case DataRP::TRAVEL_EMPTY:
        comd.setCommand(CT_BufIOBit);
        comd.setStatusValue(1,0);
        m_commds.push_back(comd);

        comd.clear();
        comd.setCommand(CT_SetSynVel);
        comd.setsynVel(m_speed.getFillingVel());
        m_commds.push_back(comd);
        //move to position.
        comd.clear();
        comd.setCommand(CT_LnXYZG0);
        comd.setLnXYZ(item.getEndPos());
        m_commds.push_back(comd);
        //set outline Vel
        comd.setCommand(CT_SetSynVel);
        comd.setsynVel(m_speed.getOutlineVel());
        m_commds.push_back(comd);
        comd.clear();

        //turn on laser.
        comd.clear();
        comd.setCommand(CT_BufIOBit);
        comd.setStatusValue(1,1);
        m_commds.push_back(comd);
     break;

     case DataRP::TRAVEL_PRINT:
       if(item.getType() == RP_ARCP)
       {
            comd.setCommand(CT_ArcXYP);
            comd.setArcXYP(item.getEndPos().getX(),item.getEndPos().getY(),item.getRadius(),(double)item.getDirection(),item.getCenter().getX(),item.getCenter().getY());
            m_commds.push_back(comd);
       }
       else if(item.getType() == RP_ARC)
       {
            comd.setCommand(CT_ArcXY);
            comd.setArcXY(item.getCenter().getX(),item.getCenter().getY(),item.getRadian(),item.getRadius(),item.getEndPos().getX(),item.getEndPos().getY());
            m_commds.push_back(comd);
       }

       else if(item.getType() == RP_LINE)
       {
            comd.setCommand(CT_LnXY);
            comd.setLnXY(item.getEndPos().getX(),item.getEndPos().getY());
            m_commds.push_back(comd);
       }
     break;

     case DataRP::TRAVEL_LASEROFF:
        comd.setCommand(CT_BufIOBit);
        comd.setStatusValue(1,0);
        m_commds.push_back(comd);
     break;

     default:
     break;
    }
}

//!deal extracted paths.
void RPControl::dealPath(Line &line)
{
    RPCommd comd;
    Vec3D tpoint;

    //turn off laser.
    comd.clear();
    comd.setCommand(CT_BufIOBit);
    comd.setStatusValue(1,0);
    m_commds.push_back(comd);

    comd.clear();
    comd.setCommand(CT_SetSynVel);
    comd.setsynVel(m_speed.getFillingVel());
    m_commds.push_back(comd);
    //move to position.
    comd.clear();
    comd.setCommand(CT_LnXYZG0);
    tpoint = line.getHead();
    comd.setLnXYZ(tpoint);
    m_commds.push_back(comd);

    //turn on laser.
    comd.clear();
    comd.setCommand(CT_BufIOBit);
    comd.setStatusValue(1,1);
    m_commds.push_back(comd);

    //print.
    comd.clear();
    comd.setCommand(CT_LnXYZ);
    tpoint = line.getTail();
    comd.setLnXYZ(tpoint);
    m_commds.push_back(comd);

    //turn off laser.
    comd.clear();
    comd.setCommand(CT_BufIOBit);
    comd.setStatusValue(1,0);
    m_commds.push_back(comd);
}

/************thread**************************/

void RPControl::proPrintAll(RPControl *rpctl)
{
    rpctl->printAll();
}

void RPControl::proPushCommd(RPControl *rpctl)
{
    rpctl->getCommds();
}

void RPControl::handleResults(QString str)
{
    if(!b_printRun)
    {
        hideAllMoveControl(false);
    }

    qDebug()<<str<<endl;
}

/*************buff*****************/

void RPControl::BuffOn()
{
    //GT_HookCommand(CommandHandle);
    setLaser();
    GT_StrtList();               //buff mode start

    Vec3D firstPos = rp_data->getFirstPosition();
    firstPos.setZ(firstPos.getZ()+50);
    double start_pos[4];
    start_pos[0] = mm2pulse(firstPos.getX());
    start_pos[1] = mm2pulse(firstPos.getY());
    start_pos[2] = mm2pulse(firstPos.getZ());
    start_pos[3] = 0;
    shouldMoveOffset(start_pos);

    GT_MvXYZ(start_pos[0], start_pos[1], start_pos[2], velConvert(m_speed.getMoveVel()), accConvert(m_speed.getMoveAcc()));//initiate
    rp_simu->setSpeed(m_speed);

    GT_EndList();    //close buff
    GT_StrtMtn();    //run
    qDebug()<<"    ---buff is opened---"<<endl;
}

void RPControl::clearBuff()
{
    b_pause = true;
    rp_simu->setPause(true);
    //pause steady.
#ifdef WIN32
           Sleep(400);
#else
           usleep(400000);
#endif

    b_printRun = false;
    rp_simu->terminatePrint();
    rp_simu->eraseTracks();
    //moveXYZ(0,0,m_maxZ*RPConfig::content.getPulseEquivalent());
    QMessageBox::information(0,QObject::tr("print task"), QObject::tr("deleted"));
}

void RPControl::BuffOff()
{
    b_pause = true;
    rp_simu->setPause(true);
}

void RPControl::BuffContinue()
{
    //GT_HookCommand(CommandHandle);
    rp_simu->setPause(false);
    b_pause = false;
}

/******print********/
//!print
void RPControl::printAll()
{
    RPCommd commd;
    double prf_pos[4];
    unsigned short status = 0;
    m_loopCnt = 0;
    disableBeginSyn();

    qDebug()<<"    print size of command is "<<m_commds.size()<<endl;
    //Loop
    while(b_printRun)
    {
        if(b_pause)
        {
            if(laserAttr->canbePush())
            {
                GT_StpMtn();
                if(getLaserStatus())
                {
                    laserAttr->push(DataRP::LASER_ON);
                    switchLaser(false);
                }
                else
                {
                    laserAttr->push(DataRP::LASER_OFF);
                }
//for stop steady.
#ifdef WIN32
           Sleep(200);
#else
           usleep(200000);
#endif
                if(!isSimulate())
                {
                    GT_GetPrfPnt(prf_pos);
                    m_position.setZ(prf_pos[2]);
                }
            //in case of judge laser status delay.
            if(getLaserStatus())
            {
                if(laserAttr->getLaserStatus() == DataRP::LASER_OFF)
                {
                    laserAttr->setLaserStatus(DataRP::LASER_ON);
                    switchLaser(false);
                }
            }

            }//can be push.
//pause stay more.
#ifdef WIN32
           Sleep(400);
#else
           usleep(400000);
#endif

            continue;
        }

        if(laserAttr->canbePop())
        {
            if(!isSimulate())
            {
               long pos;
               pos = m_position.getZ();
               GT_SetAtlPos(3,pos);
            }
            DataRP::LaserStauts lstatus = laserAttr->pop();
            if(lstatus == DataRP::LASER_ON)
            {
                switchLaser(true);
            }
            GT_RestoreMtn();
        }

        if(b_runtimeSet)
        {
            if(!isSimulate())
            {
                if(m_laserPowerMode == LPMODE_PWM)
                {
                    GT_LaserPrfCmd(m_dutyratio,LASER_CHANNEL);
                }
                else if(m_laserPowerMode == LPMODE_DA)
                {
                    GT_LaserPrfCmd(m_voltage,LASER_CHANNEL);
                }

                GT_Override(m_override);
            }
            rp_simu->GT_Override(m_override);
            b_runtimeSet = false;
            qDebug()<<"set in time"<<m_override<<endl;
#ifdef WIN32
           Sleep(800);
#else
           usleep(800000);
#endif
        }

        if(isSimulate())
        {
            rp_simu->GT_GetCrdSts(&status);
        }
        else
        {
            GT_GetCrdSts(&status);
        }

        if(b_waitQuit)
        {
            if(m_commds.size() == 0 && (status&0x01) && (status && 0x2000) )
            {
#ifdef WIN32
                    Sleep(100);
#else
                    usleep(100000);
#endif
                    break;
            }
        }

        if(m_loopCnt > 5000 && isBeginSyn())//100000
        {
            m_loopCnt = 0;
            rp_data->syntoPrinting();
        }

        if(m_commds.size() >  0)
        {
            m_mutex.lock();
            commd = m_commds.front();
            m_mutex.unlock();
            //qDebug()<<"GT command "<<commd.getCommand()<<endl;

           if(commd.getCommand() == CT_BuffOn)
           {
                qDebug()<<"        to open buff "<<endl;
                do_BuffOn();
                continue;
           }

            GT_AddList();

            //qDebug()<<"GT command "<<commd.getCommand()<<endl;
            switch(commd.getCommand())
            {
                case CT_BufIOBit:
                    do_BufIOBit(commd);
                break;

               case CT_SetSynVel:
                   do_setSynVel(commd);
               break;

               case CT_LnXYZG0:
                    do_LnXYZG0(commd);
                break;

                case CT_LnXYZ:
                    do_LnXYZ(commd);
                break;

                case CT_ArcXYP:
                    do_arcXYP(commd);
                break;

                case CT_ArcXY:
                    do_arcXY(commd);
                break;

                case CT_LnXY:
                    do_LnXY(commd);
                break;

                 case CT_Finish:
                    do_Finish(commd);
                break;

                case CT_OVERLAYER:
                    do_layerOver(commd);
                break;

                default:
                    m_mutex.lock();
                    m_commds.pop_front();
                    m_mutex.unlock();
                break;
            }
            GT_EndList();
        }
        m_loopCnt++;
    }//loop

    m_loopCnt = 0;
    rp_data->m_curPrintLevel = rp_data->getMaxLevel();
    b_printRun = false;
    b_waitQuit = false;
    clearCommds();
    laserAttr->pop();
    GT_StrtList();
    GT_EndList();
    refresh();
    //go to origin point.
    setMoveSpeed();
    moveXYZ(0,0,5e4);

    qDebug()<<"print loop over"<<endl;
}

void RPControl::startPrint()
{
    if(rp_data->m_SlcSlices.size()==0)
        return;

    GT_StrtList();
    GT_EndList();
    clearCommds();
    laserAttr->clear();
    b_printRun = true;
    b_waitQuit = false;
    b_pause = false;
    hideAllMoveControl(true);
    rp_simu->init(m_speed);
    qDebug()<<"           ========start printing========"<<endl;

    BuffThread *pushCommdThread = new BuffThread();
    pushCommdThread->setProcess(&proPushCommd,this,QObject::tr("--------command ok"));
    connect(pushCommdThread, SIGNAL(resultReady(QString)), this, SLOT(handleResults(QString)));
    connect(pushCommdThread, &BuffThread::finished, pushCommdThread, &QObject::deleteLater);
    pushCommdThread->start();


    BuffThread *printThread = new BuffThread();
    printThread->setProcess(&proPrintAll,this,QObject::tr("--------print ok"));
    connect(printThread, SIGNAL(resultReady(QString)), this, SLOT(handleResults(QString)));
    connect(printThread, &BuffThread::finished, printThread, &QObject::deleteLater);
    printThread->start();

    m_minZ = rp_data->getMinZ();
    m_maxZ = rp_data->getMaxZ();
    rp_data->resetCurPrintLayer();
    rp_simu->listernCommends();
}

bool RPControl::isBeginSyn()
{
    if(!b_alreadyBeginSyn)
    {
        if(getLaserStatus())
        {
            b_alreadyBeginSyn = true;
        }
    }
    return b_alreadyBeginSyn;
}

 void RPControl::disableBeginSyn()
 {
     b_alreadyBeginSyn = false;
 }

  void RPControl::hideAllMoveControl(bool bhide)
  {
      ui_control->hideAllMoveControl(bhide);
  }

/**************process command******************/

bool RPControl::dealBlockBuff(short isblock,RPCommd &commd)
{
    unsigned short status;
    if(isblock)
    {
         GT_GetCmdSts(&status);
         if(status == 32768) // buff is full.
         {
             //qDebug()<<"buff is full"<<endl;
             return false;
         }
    }
    rp_simu->pushCommands(commd);
    m_mutex.lock();
    m_commds.pop_front();
    m_mutex.unlock();
    return true;
}

//! buff printing
void RPControl::do_BuffOn()
{
    BuffOn();
    m_mutex.lock();
    m_commds.pop_front();
    m_mutex.unlock();
}

void RPControl::do_setSynVel(RPCommd &commd)
{
    short rtn;
    double sv;
    commd.getValue(sv);
    rtn = GT_SetSynVel(velConvert(sv));
    dealBlockBuff(rtn,commd);

}

void RPControl::do_BufIOBit(RPCommd &commd)
{
    short rtn;
    unsigned short io_bit;
    short bit_status;
    double tx,ty;

    commd.getValue(tx,ty);
    io_bit = (unsigned short)tx;
    bit_status = (short)ty;

    rtn = GT_BufIOBit(io_bit,bit_status);
    dealBlockBuff(rtn,commd);
}

void RPControl::do_LnXYZG0(RPCommd &commd)
{
    short rtn;
    double tx,ty,tz;

    commd.getValue(tx,ty,tz);
    rtn = GT_LnXYZG0(mm2pulse(tx),mm2pulse(ty),mm2pulse(tz));
    dealBlockBuff(rtn,commd);
}

void RPControl::do_LnXYZ(RPCommd &commd)
{
    short rtn;
    double tx,ty,tz;

    commd.getValue(tx,ty,tz);
    rtn = GT_LnXYZ(mm2pulse(tx),mm2pulse(ty),mm2pulse(tz));
    dealBlockBuff(rtn,commd);
}

void RPControl::do_arcXYP(RPCommd &commd)
{
    short rtn;
    double tx,ty,tr,td;
    short direction;

    commd.getValue(tx,ty,tr,td);
    direction = short(td);
    rtn = GT_ArcXYP(mm2pulse(tx),mm2pulse(ty),mm2pulse(tr),direction);
    dealBlockBuff(rtn,commd);
}

void RPControl::do_arcXY(RPCommd &commd)
{
    short rtn;
    double tx,ty,radian,tangle;

    commd.getValue(tx,ty,radian);
    tangle = RAD2DEG(radian);

    rtn = GT_ArcXY(mm2pulse(tx),mm2pulse(ty),tangle);
    dealBlockBuff(rtn,commd);
}

void RPControl::do_LnXY(RPCommd &commd)
{
    short rtn;
    double tx,ty;

    commd.getValue(tx,ty);
    rtn = GT_LnXY(mm2pulse(tx),mm2pulse(ty));
    dealBlockBuff(rtn,commd);
}

void RPControl::do_Finish(RPCommd &commd)
{
    short rtn;
    double tx,ty,tz;
    commd.getValue(tx,ty,tz);
    rtn = GT_LnXYZ(mm2pulse(tx),mm2pulse(ty),mm2pulse(tz));
    if(dealBlockBuff(rtn,commd))
    {
        b_waitQuit = true;
    }
}

void RPControl::do_layerOver(RPCommd &commd)
{
    rp_simu->pushCommands(commd);
    m_mutex.lock();
    m_commds.pop_front();
    m_mutex.unlock();
}

void RPControl::controlDebug()
{
     qDebug()<<"command size"<<rp_simu->m_simucommds.size()<<endl;
     rp_simu->listernCommends();
     qDebug()<<""<<endl;
}


/************** go home ******************/
//! ->home   <- back home  -> back index  <- index
short RPControl::Home(unsigned short axis,long pos,long offset,double vel_high,double vel_low)
{
    if(b_printRun)
        return 1;

    if(isSimulate())
    {
        rp_simu->goHome(axis);
        return 0;
    }

    unsigned short stat = 0, axes;
    double prf_pos[4];
    unsigned short status,crd_status;
    //set capture home
    GT_ClrSts(axis);
    GT_CaptHome(axis);

    //get axis status
    GT_GetSts(axis,&stat);

#ifdef DEBUG
    if(stat & 0x4000)
    {
       qDebug()<<"axis "<<axis<<" Home enable"<<endl;
    }
#endif

    //high speed to home.
    GT_SetSynVel(vel_high);
    GT_SetSynAcc(0.01);

    //set every axis' current postion to zero.
    for(axes = 1; axes < 5;axes++)
    {
        GT_ZeroPos(axes);
        prf_pos[axes-1] = 0;
    }
    //move the current axis.
    prf_pos[axis-1] = pos;
    GT_LnXYZ(prf_pos[0],prf_pos[1],prf_pos[2]);

    do                              //---trigger home
    {
        GT_GetCrdSts(&crd_status);
        if(crd_status&1)
        {
            return 1; //stop move.
        }

        GT_GetSts(axis,&status);

    }while(!(status&0x8));//trigger home.
    //smoothly stop path movtion.
    GT_StpMtn();

    do                             //---slide.
    {
        GT_GetCrdSts(&crd_status);

    }while(!(crd_status&1));

    //get the captured postion.
    GT_GetCapt(axis,&pos);
    //low speed with index.
    GT_SetSynVel(vel_low);
    //reverse direction.
    prf_pos[axis-1]= pos;
    GT_LnXYZ(prf_pos[0],prf_pos[1],prf_pos[2]);

    do                            //--- slowly move back to home postion.
    {
        GT_GetCrdSts(&crd_status);

    }while(!(crd_status&1));

    GT_ClrSts(axis);
    //set index capture.
    GT_CaptIndex(axis);

    //continue move on.
    //qDebug()<<"axis "<<axis<<" index enable"<<endl;
    prf_pos[axis-1]+= m_rpp +10;
    GT_LnXYZ(prf_pos[0],prf_pos[1],prf_pos[2]);

    do                             //--- continie move back for index trigger.
    {
        GT_GetCrdSts(&crd_status);

        if(crd_status&1)
        {
            return 1;
        }
        GT_GetSts(axis,&status);

    }while(!(status&0x8)); //index trigger.

    GT_StpMtn();

    do                         //--- slowly move back stop.
    {
        GT_GetCrdSts(&crd_status);

    }while(!(crd_status&1));

    GT_GetCapt(axis,&pos);
    GT_SetSynVel(vel_low/2);
    prf_pos[axis-1]= pos;
    GT_LnXYZ(prf_pos[0],prf_pos[1],prf_pos[2]);
    //qDebug()<<"axis "<<axis<<"move to index"<<endl;

    do                         //--- slowly move to index position.
    {
        GT_GetCrdSts(&crd_status);

    }while(!(crd_status&1));

    //qDebug()<<"axis "<<axis<<"move to offset"<<endl;
     //prf_pos[axis-1] += offset;
    //for stop steady.
    #ifdef WIN32
               Sleep(500);
    #else
               usleep(500000);
    #endif
    GT_ZeroPos(axis);
    GT_SetSynVel(vel_low*2);
    prf_pos[axis-1] = offset;
    GT_LnXYZ(prf_pos[0],prf_pos[1],prf_pos[2]);

    do                         //--- quickly move offset.
    {
        GT_GetCrdSts(&crd_status);

    }while(!(crd_status&1));

#ifdef WIN32
           Sleep(500);
#else
           sleep(500000);
#endif
    GT_ZeroPos(axis);
    //restore to original status.
   setMoveSpeed();

    rp_simu->goHome(axis);
    updateDisplay();
    return 0;
}

void RPControl::goOrigin()
{
    short rnt = 0;
     rnt  = Home(1,m_seekOrigin[0],0,20,4);
     rnt += Home(2,m_seekOrigin[1],0,20,4);
     rnt += Home(3,m_seekOrigin[2],0,20,4);
     if(rnt != 0)
     {
         QString strErr = " ";
         QMessageBox::critical(0,QObject::tr("origin error"),strErr);
     }
}

void RPControl::goHome()
{
    //AxisInitial(3,0xFF);
    short rnt = 0;
    //GT_HookCommand(CommandHandle);
    //qDebug()<<"home axis:"<<m_homeAxis<<endl;
    //axis, coordination, offset, high velocity, low velocity
    if(m_homeAxis & AXIS_BZ)
    {
       rnt +=Home(3,m_seekOrigin[2],m_origin[2],20,4);
    }

    if(m_homeAxis & AXIS_BY)
    {
       rnt += Home(2,m_seekOrigin[1],m_origin[1],20,4);
    }

    if(m_homeAxis & AXIS_BX)
    {
        rnt += Home(1,m_seekOrigin[0],m_origin[0],20,4);
    }

    if(rnt != 0)
    {
        QString strErr = " ";
        QMessageBox::critical(0,QObject::tr("Home error"),strErr);
    }

    QString info;
    Vec4D pos;
    getCurrentPosition(pos);
    for(int i = 0; i < 3; i++)
    {
     info.append(QString::number(pos.getValue(i),4,2));
     if(i != 2)
        info.append(",  ");
    }
    emit showInfo(info);
}

void RPControl::setui(WTabControl *uip)
{
    ui_control = uip;
}

void RPControl::setLaserPowerMode(unsigned short smode)
{
    m_laserPowerMode = smode;
}

unsigned short RPControl::getLaserPowerMode()
{
    return m_laserPowerMode;
}

void  RPControl::setOrigin()
{
   //GT_HookCommand(CommandHandle);
   double prf_pos[4];
   GT_GetPrfPnt(prf_pos);
   RPConfig::content.setOrigin(prf_pos);
   RPInitiate init;
   init.recordConfig();
   resetZero();
   RPConfig::content.getOrigin(m_origin[0],m_origin[1],m_origin[2]);
}

void RPControl::resetZ()
{
    if(b_printRun)
        return;

    GT_ZeroPos(3);
    rp_simu->goHome(3);
    QString info;
    Vec4D pos;
    getCurrentPosition(pos);
    for(int i = 0; i < 3; i++)
    {
     info.append(QString::number(pos.getValue(i),4,2));
     if(i != 2)
        info.append(",  ");
    }
    emit showInfo(info);
}

void RPControl::resetZ2Bigin()
{
    Vec3D firstPos = rp_data->getFirstPosition();
    double firstZpos;
    firstZpos = mm2pulse(firstPos.getZ());
    GT_SetAtlPos(3,firstZpos);

    rp_simu->GT_SetAtlPos(3,firstPos.getZ());
    QString info;
    Vec4D pos;
    getCurrentPosition(pos);
    for(int i = 0; i < 3; i++)
    {
     info.append(QString::number(pos.getValue(i),4,2));
     if(i != 2)
        info.append(",  ");
    }
    emit showInfo(info);
}

void  RPControl::saveConfig()
{

 if(m_laserPowerMode == LPMODE_PWM)
 {
     RPConfig::content.setLaserPowerPWM(m_dutyratio);
 }
 else if(m_laserPowerMode == LPMODE_DA)
 {
    RPConfig::content.setLaserPowerDA(m_voltage);
 }

 RPConfig::content.setMoveVel(m_speed.getMoveVel());
 RPConfig::content.setMoveAcc(m_speed.getMoveAcc());
 RPConfig::content.setOutlineVel(m_speed.getOutlineVel());
 RPConfig::content.setLaserDelay(laser_offDelay,laser_onDelay);
}
