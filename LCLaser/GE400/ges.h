typedef void (__stdcall CCommandHandle)(char *,short);

#if !defined(__GTDLL_H__)
#define __GTDLL_H__

#define GTDLL_API extern "C" __declspec(dllimport) short __stdcall
#define GTDLL_VARIABLE	   __declspec(dllimport)

/*************Base GE Command Start**************/
GTDLL_API  GT_AddList(void);
GTDLL_API  GT_AddLookData(char Code,char PlaneGroup,double r,double x,double y,double z,double F,double cx,double cy,int i,long n,short flag);
GTDLL_API  GT_AlarmOff(unsigned short axis);
GTDLL_API  GT_AlarmOn(unsigned short axis);
GTDLL_API  GT_ArcXY(double x_center,double y_center,double angle);
GTDLL_API  GT_ArcXYP(double x_end,double y_end,double r,short dir);
GTDLL_API  GT_AuStpOff(unsigned short axis);
GTDLL_API  GT_AuStpOn(unsigned short axis);
GTDLL_API  GT_AxisOff(unsigned short axis);
GTDLL_API  GT_AxisOn(unsigned short axis);
GTDLL_API  GT_BufIO(unsigned short value);
GTDLL_API  GT_BufIOBit(unsigned short io_bit,short bit_status);
GTDLL_API  GT_CalVel(double *vel,long *num);
GTDLL_API  GT_CalVelEx(double *Vend,long *num,double *Vfeed);
GTDLL_API  GT_CaptHome(unsigned short axis);
GTDLL_API  GT_CaptIndex(unsigned short axis);
GTDLL_API  GT_CaptProb(void);
GTDLL_API  GT_Close(void);
GTDLL_API  GT_ClrEncPos(unsigned short encoder);
GTDLL_API  GT_ClrSts(unsigned short axis);
GTDLL_API  GT_Delay(long delay_time);
GTDLL_API  GT_DrvRst(unsigned short axis);
GTDLL_API  GT_EncVel(unsigned short encoder,double *vel);
GTDLL_API  GT_EncOff(unsigned short axis);
GTDLL_API  GT_EncOn(unsigned short axis);
GTDLL_API  GT_EncPos(unsigned short encoder,long *pos);
GTDLL_API  GT_EncSns(unsigned short value); 
GTDLL_API  GT_EndList(void);
GTDLL_API  GT_EStpMtn(void);
GTDLL_API  GT_ExInpt(unsigned short *value);
GTDLL_API  GT_ExOpt(unsigned short value);
GTDLL_API  GT_ExOptBit(unsigned short bit,unsigned short value);
GTDLL_API  GT_GetAtlPos(unsigned short axis,long *pos);
GTDLL_API  GT_GetAtlErr(unsigned short axis,long *error);
GTDLL_API  GT_GetAtlVel(unsigned short axis,double *vel);
GTDLL_API  GT_GetBrkPnt(double *pnt);
GTDLL_API  GT_GetCapt(unsigned short axis,long *value);
GTDLL_API  GT_GetCmdSts(unsigned short *value);
GTDLL_API  GT_GetCrdSts(unsigned short *coord_status);
GTDLL_API  GT_GetExOpt(unsigned short *exopt);
GTDLL_API  GT_GetHomeSwt(unsigned short *home);
GTDLL_API  GT_GetLmtSwt(unsigned short *value);
GTDLL_API  GT_GetMtnNm(unsigned short *motion_linenum);
GTDLL_API  GT_GetPosErr(unsigned short axis,unsigned short *value);
GTDLL_API  GT_GetPrfPnt(double* pnt);
GTDLL_API  GT_GetPrfVel(double *vel);
GTDLL_API  GT_GetSegPnt(double* pnt);
GTDLL_API  GT_GetSts(unsigned short axis, unsigned short *value);
GTDLL_API  GT_HardRst(void); 
GTDLL_API  GT_HomeSns(unsigned short value);
GTDLL_API  GT_HookCommand(CCommandHandle *CommandHandle);
GTDLL_API  GT_IndexSns(unsigned short value);
GTDLL_API  GT_InitLookAhead(double T,double amax,double am,double vmax,int n,double con);
GTDLL_API  GT_LmtSns(unsigned short value);
GTDLL_API  GT_LmtsOff(unsigned short axis);
GTDLL_API  GT_LmtsOn(unsigned short axis);
GTDLL_API  GT_LnXY(double x, double y);
GTDLL_API  GT_LnXYG0(double x, double y);
GTDLL_API  GT_MapCnt(unsigned short axis,double Count);
GTDLL_API  GT_MvXY(double x,double y,double vel,double accel);
GTDLL_API  GT_Open(unsigned long PortBase=65535);
GTDLL_API  GT_Override(double value);          //laser scanner do not have this function
GTDLL_API  GT_OverrideG0(double value);        //laser scanner do not have this function
GTDLL_API  GT_Reset(void);
GTDLL_API  GT_RestoreMtn(void);
GTDLL_API  GT_RstSts(unsigned short Axis,unsigned short value);
GTDLL_API  GT_SetAtlPos(unsigned short AxisNum, double pos);
GTDLL_API  GT_SetDccVel(double vel);
GTDLL_API  GT_SetMaxVel(double value);
GTDLL_API  GT_SetPosErr(unsigned short axis,unsigned short value);
GTDLL_API  GT_SetStpAcc(double value);
GTDLL_API  GT_SetEStpAcc(double value);
GTDLL_API  GT_SetStrtVel(double value);
GTDLL_API  GT_SetSynAcc(double accel);
GTDLL_API  GT_SetSynVel(double vel);
GTDLL_API  GT_StepDir(unsigned short axis);
GTDLL_API  GT_StepPulse(unsigned short axis);
GTDLL_API  GT_StpMtn(void);
GTDLL_API  GT_StrtList(void);
GTDLL_API  GT_StrtMtn();
GTDLL_API  GT_Update(unsigned short axis);
GTDLL_API  GT_ZeroPos(unsigned short AxisNum);

/*************HandWheel**************/
GTDLL_API  GT_HandWheel(unsigned short axis,double value);
GTDLL_API  GT_ExitHWheel(void);

/*************ADC**************/
/*********************this function is not available for standard card**********************/
GTDLL_API  GT_GetAdc(unsigned short channel, short* D_data);
GTDLL_API  GT_SetAdcChn(unsigned short value); 

/*************multi card**************/
/*********************this function is for multi-card **********************/ 
GTDLL_API  GT_SwitchtoCardNo(unsigned short number);          
GTDLL_API  GT_GetCurrentCardNo(void);

/*************voltage function**************/
GTDLL_API  GT_CloseSpindle(void);
GTDLL_API  GT_CtrlMode(unsigned short axis,unsigned short value);
GTDLL_API  GT_GetIntgr(unsigned short axis,short *value);
GTDLL_API  GT_GetILmt(unsigned short axis,unsigned short *limit);
GTDLL_API  GT_GetKaff(unsigned short axis,double *kaff);
GTDLL_API  GT_GetKd(unsigned short axis,double *kd);
GTDLL_API  GT_GetKi(unsigned short axis,double *ki);
GTDLL_API  GT_GetKp(unsigned short axis,double *kp);
GTDLL_API  GT_GetKvff(unsigned short axis,double *kvff);
GTDLL_API  GT_GetMtrBias(unsigned short axis,short *bias);
GTDLL_API  GT_GetMtrCmd(unsigned short axis,short *voltage);
GTDLL_API  GT_GetMtrLmt(unsigned short axis,unsigned short *limit);
GTDLL_API  GT_SetILmt(unsigned short axis,unsigned short limit);
GTDLL_API  GT_SetKaff(unsigned short axis,double kaff);
GTDLL_API  GT_SetKd(unsigned short axis,double kd);
GTDLL_API  GT_SetKi(unsigned short axis,double ki);
GTDLL_API  GT_SetKp(unsigned short axis,double kp);
GTDLL_API  GT_SetKvff(unsigned short axis,double kvff);
GTDLL_API  GT_SetMtrBias(unsigned short axis,short bias);
GTDLL_API  GT_SetMtrLmt(unsigned short axis,unsigned short limit);
GTDLL_API  GT_SetSpindleVel(short value);

/************GE300SX only***************/
GTDLL_API  GT_ArcYZ(double y_center,double z_center,double angle);
GTDLL_API  GT_ArcYZP(double y_end,double z_end,double r,short direction);
GTDLL_API  GT_ArcZX(double z_center,double x_center,double angle);
GTDLL_API  GT_ArcZXP(double z_end,double x_end,double r,short direction);
GTDLL_API  GT_LnXYZ(double x,double y,double z);
GTDLL_API  GT_LnXYZG0(double x,double y,double z);
GTDLL_API  GT_MvXYZ(double x,double y,double z,double vel,double accel);

/************GE400SX only***************/
GTDLL_API  GT_LnXYZA(double x,double y,double z,double a);
GTDLL_API  GT_LnXYZAG0(double x,double y,double z,double a);
GTDLL_API  GT_MvXYZA(double x,double y,double z,double a,double vel,double accel);

/*************laser control Command Start*************/
GTDLL_API GT_GetHSIOOpt(unsigned short *exopt);
GTDLL_API GT_GetHSIOSts(short* HSIO_BUF_STS);
GTDLL_API GT_SetHSIOOpt(unsigned short value);
GTDLL_API GT_SetLevelDelay(unsigned short highleveldelay,unsigned short lowleveldelay);
GTDLL_API GT_SetHsioManageMode(unsigned short mode);
GTDLL_API GT_EnaFPK(unsigned short time1, unsigned short time2, unsigned short laser_off_delay);
GTDLL_API GT_DisFPK(void);
GTDLL_API GT_SetLaserMode(unsigned short laserChannel, unsigned short laserMode);
GTDLL_API GT_LaserPowerMode(unsigned short laser_power_mode,double max_value,double min_value,unsigned short laserChannel=1);
GTDLL_API GT_LaserPowerModeEx(unsigned short laser_power_mode, double MaxLaserPower, double MinLaserPower);
GTDLL_API GT_LaserPrfCmd(double output_cmd, unsigned short laserChannel=1);
GTDLL_API GT_LaserPrfCmdEx(double PrfCmd);
GTDLL_API GT_LaserOutFrq(double output_frequency, unsigned short laserChannel=1);
GTDLL_API GT_LaserOutFrqEx(double OutFrq);
GTDLL_API GT_LaserPowerOffset(double offset, unsigned short laserChannel=1);
GTDLL_API GT_LaserPowerOffsetEx(double Offset);
GTDLL_API GT_LaserPrfFollow(double follow_ratio);
GTDLL_API GT_LaserPrfFollowEx (short LaserChannel,double Kp, double MaxLaserPower, double MinLaserPower);
GTDLL_API GT_LaserPowerOverride(unsigned short laserChannel, double override);
GTDLL_API GT_SetPulseWidth(unsigned short channel, unsigned short width);
GTDLL_API GT_SetWaitPulse (unsigned short LaserChannel, unsigned short Mode, double WaitPulseFrq, double WaitPulseDuty);
GTDLL_API GT_SetPreVltg(unsigned short LaserChannel, unsigned short Mode, double voltage);
GTDLL_API GT_SetLPFMode(short LPFllwMode, unsigned short velMode=0);
GTDLL_API GT_LoadLPTable(double StartVel, double BasicVel, unsigned short Length, double *LasPwrFile);
GTDLL_API GT_SetCmpMode(short mode);
GTDLL_API GT_StartCmpFunc( short valid_level, unsigned short axis, long *pos,double vel, double acc, \
              unsigned short pixel_length,unsigned short data_length, long* data_file,\
              double* strtlaser_file,double* endlaser_file, short *switch_file,unsigned short cmpSource=0);
GTDLL_API GT_ExDAC(unsigned short channel, double DacValue);
GTDLL_API GT_BufDAC(unsigned short channel, double DacValue);
GTDLL_API GT_AutoFocus ( short AF_MODE, double kp);
GTDLL_API GT_StartCmpFuncNew(unsigned short axis, long *pos, double vel, double acc, unsigned short data_length, long* data_file, short *laser_file,unsigned short cmpSource=0);
GTDLL_API GT_EndCmpFunc(void);

GTDLL_API GT_ExInptEx(unsigned short *value);
GTDLL_API GT_SetEstpParm(unsigned short inputPort,unsigned short validSense);

/*************special function**************/
GTDLL_API GT_SetBacklash(unsigned short axis,unsigned short backlash);
GTDLL_API GT_GetBufSpace( unsigned short * BufSpace );
GTDLL_API GT_SetSoftLmt(unsigned short mode, long lmtPosX, long lmtPosY);
GTDLL_API GT_SetAxisScale(double ScaleX, double ScaleY, double ScaleZ, double ScaleA);
GTDLL_API GT_GetPrfPntEx(double* pnt);

/*************gantry function**************/
GTDLL_API GT_GantryMode(unsigned short slave,unsigned short master);
GTDLL_API GT_ExitGantryMode(void);
GTDLL_API GT_GantryPrm(unsigned short error,unsigned short range=0,double kp=0);

/*************special function**************/
GTDLL_API GT_GetDSPVrsn(unsigned long *version);
GTDLL_API GT_GetFpgaVrsn(unsigned long *version);
GTDLL_API GT_GetDllVrsn(unsigned  long *version);

/************LookAhead function***********/
typedef struct 
{
	long mtnNum;//返回在哪条运动指令出错
	short num;//返回在出错的运动指令前第几条指令没有发送成功
	short error;//返回出错原因
}TLookAheadMsg;

GTDLL_API GT_SwitchLookaheadMode(unsigned short mode);
GTDLL_API GT_EndLookaheadMode(void);
GTDLL_API GT_InitLookAheadEx(double T,double amax,double am,short n,double con);
GTDLL_API GT_InitLookAheadExx(double T,double amax,double am,short n,double con,double timepara,char* filename);
GTDLL_API GT_MvXYEx(double x, double y, double vel);
GTDLL_API GT_MvXYZEx(double x, double y, double z, double vel);
GTDLL_API GT_LnXYG0Ex(double x, double y, double F, short dccFlag, long CodeNum, TLookAheadMsg *pMsg);
GTDLL_API GT_LnXYZG0Ex(double x, double y,  double z, double F, short dccFlag, long CodeNum, TLookAheadMsg *pMsg );
GTDLL_API GT_LnXYEx(double x, double y, double F, short dccFlag, long CodeNum, TLookAheadMsg *pMsg);
GTDLL_API GT_LnXYZEx(double x, double y, double z, double F, short dccFlag, long CodeNum, TLookAheadMsg *pMsg);
GTDLL_API GT_ArcXYPEx(double x_end,double y_end,double r,short direction,double F, short dccFlag,long CodeNum,TLookAheadMsg *pMsg);
GTDLL_API GT_ArcYZPEx(double y_end,double z_end,double r,short direction,double F, short dccFlag,long CodeNum,TLookAheadMsg *pMsg);
GTDLL_API GT_ArcZXPEx(double z_end,double x_end,double r,short direction,double F, short dccFlag,long CodeNum,TLookAheadMsg *pMsg);
GTDLL_API GT_ArcXYEx(double x_center,double y_center,double angle, double F, short dccFlag, long CodeNum, TLookAheadMsg *pMsg);
GTDLL_API GT_ArcYZEx(double y_center,double z_center,double angle, double F, short dccFlag, long CodeNum, TLookAheadMsg *pMsg);
GTDLL_API GT_ArcZXEx(double z_center,double x_center,double angle, double F, short dccFlag, long CodeNum, TLookAheadMsg *pMsg);
GTDLL_API GT_BufIOBitEx(unsigned short io_bit, short bit_status);
GTDLL_API GT_BufIOEx(unsigned short status);
GTDLL_API GT_DelayEx(long time);
GTDLL_API GT_M30(TLookAheadMsg *pMsg);
#endif //__GTDLL_H__