/*************GE Command****************************/
#ifndef GES_H
#define GES_H

typedef void CCommandHandle(char*,short int);

/*************call back function**************/
int GT_HookCommand(CCommandHandle *CommandHandle);


/*************Base GE Command Start**************/
int GT_Open();
int GT_Close();

int GT_Reset();

int GT_AlarmOn(unsigned short axis);
int GT_AlarmOff(unsigned short axis);

int GT_EncOn(unsigned short axis);
int GT_EncOff(unsigned short axis);

int GT_AxisOn(unsigned short axis);
int GT_AxisOff(unsigned short axis);

int GT_GetCmdSts(unsigned short *value);

int GT_GetSts(unsigned short axis, unsigned short *value);
int GT_ClrSts(unsigned short axis);

int GT_GetCrdSts(unsigned short *coord_status);

int GT_GetLmtSwt(unsigned short *value);

int GT_LmtsOff(unsigned short axis);
int GT_LmtsOn(unsigned short axis);
int GT_LmtSns(unsigned short value);

int GT_StepDir(unsigned short axis);
int GT_StepPulse(unsigned short axis);

int GT_SetStrtVel(double value);
int GT_SetSynAcc(double accel);
int GT_SetSynVel(double vel);

int  GT_StrtList(void);  //open and clear buffer.
int  GT_EndList(void);   //buffer command, close buffer.
int  GT_AddList(void);   //add command to buffer

int  GT_StrtMtn(void);       //start move.
int  GT_StpMtn(void);    //stop move.
int  GT_EStpMtn(void);   //halt move.
int  GT_RestoreMtn(void);    //contiune move.


int GT_ZeroPos(unsigned short AxisNum);

int  GT_BufIO(unsigned short value);  //with buffer mode, controls the 16 bit IO.
int  GT_BufIOBit(unsigned short io_bit,short bit_status);  //with buffer mode, setting high or low level of IO.

int GT_ArcXY(double x_center,double y_center,double angle);
int GT_ArcXYP(double x_end,double y_end,double r, short dir);
int GT_LnXY(double x, double y);

int GT_CaptHome(unsigned short axis);   //set capture home.
int GT_CaptIndex(unsigned short axis);  //set capture index.
int GT_GetCapt(unsigned short axis, long *value); //  get capture value.

int GT_GetPrfPnt(double *pnt);
int GT_SetAtlPos(unsigned short AxisNum,double pos);

int  GT_Override(double value);          //laser scanner do not have this function
int  GT_OverrideG0(double value);        //laser scanner do not have this function


/*************voltage function**************/
int  GT_CtrlMode(unsigned short axis,unsigned short value);

/************GE300SX only***************/

int GT_ArcYZ(double y_center,double z_center,double angle);
int GT_ArcYZP(double y_end,double z_end,double r,short direction);
int GT_ArcZX(double z_center,double x_center,double angle);
int GT_ArcZXP(double z_end,double x_end,double r,short direction);
int GT_LnXYZ(double x,double y,double z);
int GT_LnXYZG0(double x,double y,double z);
int GT_MvXYZ(double x,double y,double z,double vel,double accel);


/************GE400SX only***************/
int GT_LnXYZA(double x,double y,double z,double a);
int GT_LnXYZAG0(double x,double y,double z,double a);
int GT_MvXYZA(double x,double y,double z,double a,double vel,double accel);


/*************laser control Command Start*************/
int GT_GetHSIOOpt(unsigned short *exopt);  //getathe status of IO with high or low level.
int GT_GetHSIOSts(short* HSIO_BUF_STS);

int GT_SetHSIOOpt(unsigned short value);
int GT_LaserPowerMode(unsigned short laser_power_mode,double max_value,double min_value, unsigned short laserChannel = 1);
int GT_LaserOutFrq(double output_frequency, unsigned short laserChannel = 1);
int GT_LaserPrfCmd(double output_cmd, unsigned short laserChannel = 1);
int GT_SetLaserMode(unsigned short laserChannel, unsigned short laserMode);
int GT_SetRedLightOpt(unsigned short value);
int GT_SetLevelDelay(unsigned short highleveldelay,unsigned short lowleveldelay);


#endif  //end GES_H
