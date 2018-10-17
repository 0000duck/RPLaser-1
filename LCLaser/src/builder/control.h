/***************************************************************************
                             control.h  -  description
                             -------------------
    begin                : Apr. 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#ifndef RPControl_H
#define RPControl_H

#include <QObject>
#include <iostream>
#include <vector>
#include <QProgressDialog>
#include <QMutex>
#include "gtmock.h"
#include "command.h"
#include "config.h"
#include "matrix4x4.h"
#include "controlinterface.h"

#define LASER_CHANNEL 1

using namespace std;

typedef vector<bool> VBseq;
typedef vector<VBseq> VStatus;

class MainWindow;
class WTabControl;

namespace BuildRP
{
    enum PowerMode
    {
        LPMODE_PWM = 0,
        LPMODE_FM,
        LPMODE_DA,
    };

    enum SampePosStauts
    {
        POS_1 =1<<0,
        POS_2 =1<<1,
        POS_3 =1<<2,
        POS_4 =1<<3,

    };

    enum AXIS
    {
        AXIS_BX =1<<0,
        AXIS_BY =1<<1,
        AXIS_BZ =1<<2,
    };

    class RPControl : public ControlInterface
    {
        Q_OBJECT
    public:
        explicit RPControl(QObject *parent = 0);
        ~RPControl();

        //interface
        void setSingleTrack(int trackType);
        void setSingleHeight(int smaxHeight);
        void setSingleLenghtRadius(double svalue);
        void SetSingleDeltaZ(double sdZ);
        void singleBuffOn();
        void singleBuffOff();
        void singleBuffContinue();
        void SingleClearBuff();

        bool connectMachine();
        void disConnectMachine();
        void turnOnLaser();
        void turnOffLaser();
        void switchLaser(bool on);

        void setFrequency(double frequency);
        void setDutyratio(double dutyratio);
        void setVoltage(double voltage);
        bool getLaserStatus();
        void setui(WTabControl *uip);

        bool isBeginSyn();
        void disableBeginSyn();

        void setLaserPowerMode(unsigned short smode);
        unsigned short getLaserPowerMode();
        void move2Start();
        void move2MeasurePos(Vec3D spos);

        short Home(unsigned short axis,long pos,long offset,double vel_high,double vel_low);
        void goHome();

        //thread function.
        static void proPrintAll(RPControl *rpctl);
        static void proPushCommd(RPControl *rpctl);

        void threadMoveXYZ(double sx, double sy, double sz,double vel,double acc);

        void hideAllMoveControl(bool bhide);

        void controlDebug();

#ifdef WIN32
        static void __stdcall CommandHandle(char *command,short error);
#else
       static void CommandHandle(char *command,short error);
#endif
        void saveConfig();
        bool enableAxis();
        bool disableAxis();
        void moveXYZfromZero(double sx,double sy,double sz);
        void moveXYZfromZero(unsigned int saxis,double srange);
        void moveOffset();
        bool shouldMoveOffset(const double (&pos)[4]);
        void moveXYZ(double sx,double sy,double sz);
        void clearLimit();
        void getStatus(VStatus& status);
        void getLimitStatus();

        void setData(DataRP::RPData *pdata);
        void switchSIMU(bool isOn);
        void printAll();
        void refresh();
        void startPrint();

        void BuffOn();
        void BuffOff();
        void BuffContinue();

        void clearBuff();
        void setParaminRuntime();

        void setMoveVelValue(double sv);
        void setMoveAccValue(double sv);
        void setOutlineVelValue(double sv);
        void setFillingVelValue(double sv);
        void setOutlineFillingAccValue(double sv);

        void setLaserOnDelay(double son);
        void setLaserOffDelay(double soff);

        void setHomeAxis(unsigned short haxis);

        void resetZero();
        void setOveride(double sr);

        double getPrintZ();

        void getCurrentPosition(Vec4D &pos);

        bool isPrintRun();

        void goOrigin();
        void setOrigin();
        void resetZ();
        void resetZ2Bigin();

        double mm2pulse(double smm);
        double pulse2mm(double pulse);

        QMutex m_mutex;

    signals:

        void updateLaserStatus();
        void updateDisplay();
        void showInfo(QString info);

    public slots:
        void handleResults(QString str);

    protected:
        void setMoveSpeed();
        void setFillingSpeed();
        void setOutlineSpeed();

        void clearCommds();

        void setLaser();
        void laserWithPowerMode();

        static bool isSimulate();
        void getCommds();

        static void extractOutlines(RPControl* rpconntrol,RPprintItem &item,int &runtype);
        static void extractPaths(RPControl* rpconntrol,Line &line);

        void dealOutline(RPprintItem &item,int &runtype);
        void dealPath(Line &line);

        static bool b_simu;

        double velConvert(double vel);
        double accConvert(double acc);
        void do_BuffOn();
        void do_BufIOBit(RPCommd &commd);
        void do_setSynVel(RPCommd &commd);

        void do_arcXY(RPCommd &commd);
        void do_arcXYP(RPCommd &commd);
        void do_LnXY(RPCommd &commd);
        void do_LnXYZ(RPCommd &commd);
        void do_LnXYZG0(RPCommd &commd);
        void do_Finish(RPCommd &commd);
        void do_layerOver(RPCommd &commd);

        void drawArc();
        void drawLine();
        void drawLineZ();
        bool dealBlockBuff(short isblock,RPCommd &commd);

    private:
        double m_frequency;
        double m_dutyratio;
        double m_voltage;
        SpeedSet m_speed;
        DataRP::RPData *rp_data;
        SIMURP::GTMock *rp_simu;
        CommdList m_commds;

        double m_minZ;
        double m_maxZ;
        Vec3D m_position;
        Vec3D m_tryposition;
        double m_rpp;
        double m_pulseEqu; // pulse/mm
        bool b_pause;
        bool b_runtimeSet;
        bool b_printRun;

        unsigned short m_homeAxis;
        unsigned short m_laserPowerMode;
        bool b_waitQuit;
        DataRP::LaserAttribute *laserAttr;
        int singleTrackType;
        double singledeltaZ;
        double singleLenghtRadius;
        unsigned singleMaxLayer;
        double laser_offDelay;
        double laser_onDelay;
        double m_override;
        double m_origin[3];
        double m_seekOrigin[3];
        WTabControl *ui_control;
        int m_loopCnt;
        bool b_alreadyBeginSyn;
        Vec3D m_threadPostion;
    };
}
#endif // RPControl_H
