/***************************************************************************
                          gtmock.h  -  description
                             -------------------
    begin                : May 20 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef GTMOCK_H
#define GTMOCK_H

#include "simu.h"
#include "command.h"
#include "dataDynamic.h"

namespace SIMURP
{

    enum SimCommandType
    {
        SIM_Wait = 0,
        SIM_BuffLnXYZ,
        SIM_IMMELnXYZ,
        SIM_BufIOBit,
    };

    enum PathType
    {
        PATH_SCAN = 0,
        PATH_OUTLINE,
    };

    struct AxisStatus
    {
        bool positive_limit;
        bool negative_limit;

        AxisStatus()
        {
            positive_limit = false;
            negative_limit = false;
        }
    };

    typedef vector<AxisStatus> AxesStatus;

    class GTMock:public RPsimu
    {
        Q_OBJECT
      public:
            GTMock(DataRP::RPData *rpdata, QObject *sparent = 0);
            ~GTMock();

            void init(SpeedSet speed);
            void goHome(unsigned short axis);
            void GT_SetAtlPos(unsigned short AxisNum, double pos);
            void GT_LnXYZ(double sx,double sy,double sz);
            void GT_JumpXYZ(double sx,double sy,double sz);
            void setSpeed(SpeedSet speed, int type = 0);
            void adjustSpeed(double srate);
            int GT_ZeroPos(unsigned short axisNum);
            int GT_Override(double value);
            int GT_SetHSIOOpt(unsigned short value);
            int GT_GetCrdSts(unsigned short *coord_status);
            int GT_ClrSts(unsigned short axis);
            int GT_GetHSIOOpt(unsigned short *exopt);
            int GT_MvXYZ(double x,double y,double z,double vel,double accel);
            int GT_GetPrfPnt(double *pnt);
            int terminatePrint();


            void pushCommands(BuildRP::RPCommd &commd);
            void listernCommends();
            void setPause(bool ispause);
            void switchLaser(bool bstatus,unsigned short priority = 10);
            void eraseTracks();
            void stopTimer();

            virtual void timeConsuming();

             BuildRP::CommdList m_simucommds;

     signals:
             void printOK();

      protected:
            void parseCommand();

            void calTracks();
            void calArc();
            void checkList();

            void timerEvent(QTimerEvent *event);
            void setsynVel(BuildRP::RPCommd &commd);
            void buffLnXYZ(BuildRP::RPCommd &commd);
            void BufIOBit(BuildRP::RPCommd &commd);
            void buffArcXYP(BuildRP::RPCommd &commd);
            void buffLnXY(BuildRP::RPCommd &commd);
            void printFinish(BuildRP::RPCommd &commd);
            void synLayerSpeed();
            void moveLaserHeader();
            void buildPath();

      private:
            double m_vel;
            DataRP::MovePosition *m_position;

            AxesStatus m_axesStatus;
            int m_currCommd;
            bool b_pause;
            double m_bounding;
            int m_cnt;
            int m_pathType;
            double m_adjustRate;
            double m_overRide;
            DataRP::LaserAttribute *laserAttr;
    };
}
#endif // GTMOCK_H
