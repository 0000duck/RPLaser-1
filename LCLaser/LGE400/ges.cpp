#include "ges.h"
#include <QtGlobal>

int GT_Open()
{
  return 0;
}

int GT_Close()
{
  return 0;
}

int GT_SetHSIOOpt(unsigned short value)
{
    Q_ASSERT(value >= 0);
    return 0;
}

int GT_SetLaserMode(unsigned short laserChannel, unsigned short laserMode)
{
   Q_ASSERT(laserChannel<10 && laserMode<10);
    return 0;
}

int GT_LaserPowerMode(unsigned short laser_power_mode,double max_value,double min_value, unsigned short laserChannel)
{
  return 0;
}

int GT_LaserOutFrq(double output_frequency, unsigned short laserChannel)
{
  return 0;
}

int GT_LaserPrfCmd(double output_cmd, unsigned short laserChannel)
{
  return 0;
}


int GT_SetRedLightOpt(unsigned short value)
{
  return 0;
}

int GT_HookCommand(CCommandHandle *CommandHandle)
{
    return 0;
}

int GT_Reset()
{
    return 0;
}

int GT_EncOn(unsigned short axis)
{
    return 0;
}

int GT_EncOff(unsigned short axis)
{
    return 0;
}

int GT_AlarmOn(unsigned short axis)
{
    return 0;
}

int GT_AlarmOff(unsigned short axis)
{
    return 0;
}

int GT_LmtsOff(unsigned short axis)
{
    return 0;
}

int GT_LmtsOn(unsigned short axis)
{
    return 0;
}

int GT_SetStrtVel(double value)
{
    return 0;
}

int GT_SetSynAcc(double accel)
{
    return 0;
}

int GT_SetSynVel(double vel)
{
    return 0;
}

int GT_CtrlMode(unsigned short axis,unsigned short value)
{
    return 0;
}

int GT_GetSts(unsigned short axis, unsigned short *value)
{
    *value = 0;
    return 0;
}

int GT_ClrSts(unsigned short axis)
{
    return 0;
}

int GT_AxisOn(unsigned short axis)
{
    return 0;
}

int GT_AxisOff(unsigned short axis)
{
    return 0;
}

int GT_LmtSns(unsigned short value)
{
    return 0;
}

int GT_GetLmtSwt(unsigned short *value)
{
    *value  = 0xFF;
    return 0;
}

int GT_GetCmdSts(unsigned short *value)
{
    *value  = 0xFF;
    return 0;
}

int GT_StepDir(unsigned short axis)
{
    return 0;
}

int GT_StepPulse(unsigned short axis)
{
    return 0;
}


int GT_ZeroPos(unsigned short AxisNum)
{
    return 0;
}

int GT_ArcXYP(double x_end,double y_end,double r,short dir)
{
    return 0;
}

int GT_ArcYZ(double y_center,double z_center,double angle)
{
    return 0;
}

int GT_ArcYZP(double y_end,double z_end,double r,short direction)
{
    return 0;
}

int GT_ArcZX(double z_center,double x_center,double angle)
{
    return 0;
}

int GT_ArcZXP(double z_end,double x_end,double r,short direction)
{
    return 0;
}

int GT_LnXYZ(double x,double y,double z)
{
    return 0;
}

int GT_LnXYZG0(double x,double y,double z)
{
    return 0;
}

int GT_MvXYZ(double x,double y,double z,double vel,double accel)
{
    return 0;
}

int GT_LnXYZA(double x,double y,double z,double a)
{
    return 0;
}

int GT_LnXYZAG0(double x,double y,double z,double a)
{
    return 0;
}

int GT_MvXYZA(double x,double y,double z,double a,double vel,double accel)
{
    return 0;
}

int GT_GetCrdSts(unsigned short *coord_status)
{
    *coord_status  = 0xFF;
    return 0;
}

int  GT_StrtList(void)
{
    return 0;
}

int  GT_EndList(void)
{
    return 0;
}

int  GT_AddList(void)
{
    return 0;
}

int  GT_StrtMtn(void)
{
    return 0;
}

int  GT_StpMtn(void)
{
    return 0;
}

int  GT_EStpMtn(void)
{
    return 0;
}

int  GT_RestoreMtn(void)
{
    return 0;
}

int  GT_BufIO(unsigned short value)
{
    return 0;
}

int  GT_BufIOBit(unsigned short io_bit,short bit_status)
{
    return 0;
}

int GT_ArcXY(double x_center,double y_center,double angle)
{
    return 0;
}


int GT_LnXY(double x, double y)
{
    return 0;
}


int GT_GetHSIOOpt(unsigned short *exopt)
{
    return 0;
}


int GT_GetHSIOSts(short* HSIO_BUF_STS)
{
    return 0;
}

int GT_CaptHome(unsigned short axis)
{
    Q_ASSERT(axis >= 1);
    return 0;
}

int GT_CaptIndex(unsigned short axis)
{
    Q_ASSERT(axis >= 1);
    return 0;
}


int GT_GetCapt(unsigned short axis, long *value)
{
    Q_ASSERT(axis >= 1);
    Q_ASSERT(value != NULL);

    return 0;
}

int GT_GetPrfPnt(double *pnt)
{
    Q_ASSERT(pnt != NULL);
    return 0;
}
int GT_SetAtlPos(unsigned short AxisNum,double pos)
{
    Q_ASSERT(AxisNum >= 1);

    return 0;
}

int GT_SetLevelDelay(unsigned short highleveldelay,unsigned short lowleveldelay)
{
    Q_ASSERT(highleveldelay >= 0);
    Q_ASSERT(lowleveldelay >= 0);

    return 0;
}

int GT_Override(double value)
{
    Q_ASSERT(value>0 && value < 2.0);
    return value;
}

int GT_OverrideG0(double value)
{
    Q_ASSERT(value>0 && value < 2.0);
    return value;
}
