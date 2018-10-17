/***************************************************************************
                          tabcontrol.cpp  -  description
                             -------------------
    begin                : Apr. 2nd 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "tabcontrol.h"
#include "ui_tabcontrol.h"
#include <stdio.h>
#include "ges.h"
#include "config.h"

WTabControl::WTabControl(BuildRP::RPControl *rpcontrol, DataRP::RPData *rpdata,QWidget *parent = 0):
    QWidget(parent),
    ui(new Ui::WTabControl),
    interfacePWM(NULL),
    interfaceDA(NULL)
{
    ui->setupUi(this);

    setControl(rpcontrol);
    setData(rpdata);
    double laserOffdelay = 0, laserOndelay = 0;

    RPConfig::content.getLaserDelay(laserOffdelay,laserOndelay);

    ui->doubleSpinBox_moveVel->setValue( RPConfig::content.getMoveVel());
    ui->doubleSpinBox_moveAcc->setValue( RPConfig::content.getMoveAcc());
    ui->doubleSpinBox_outlineVel->setValue(RPConfig::content.getOutlineVel());
    ui->doubleSpinBox_fillingVel->setValue(RPConfig::content.getFillingVel());
    ui->doubleSpinBox_outlinefillingAcc->setValue(RPConfig::content.getOutlineFillingAcc());

    ui->doubleSpinBox_OffDelay->setValue(laserOffdelay);
    ui->doubleSpinBox_OnDelay->setValue(laserOndelay);

    ui->pushButton_originRecord->setEnabled(false);
}

WTabControl::~WTabControl()
{
    delete ui;

    if(interfacePWM)
        delete interfacePWM;
}

void WTabControl::setControl(BuildRP::RPControl *RPControl)
{
    rp_control = RPControl;
    rp_control->setui(this);
    //exception code place here.
    unsigned short laserPowerMode = rp_control->getLaserPowerMode();

    switch(laserPowerMode)
    {
         case BuildRP::LPMODE_PWM:
            ui->radioButton_LaserPulse->setChecked(true);
            on_radioButton_LaserPulse_clicked(true);
            break;
         case BuildRP::LPMODE_DA:
            ui->radioButton_LaserAnalog->setChecked(true);
            on_radioButton_LaserAnalog_clicked(true);
            break;
         default:
            break;
    }

    QString axes = "X,Y,Z,A";
    QStringList itemList;
    itemList = axes.split(",");
    ui->comboBox_axes->addItems(itemList);


    if(ui->checkBox_axesEnable->isChecked())
    {
      ui->push_forward->setEnabled(true);
      ui->push_backward->setEnabled(true);
    }
    else
    {
      ui->push_forward->setEnabled(false);
      ui->push_backward->setEnabled(false);
    }
}

void WTabControl::setData(DataRP::RPData *pdata)
{
     rp_control->setData(pdata);
}

void WTabControl::setParam()
{
    rp_control->setLaserOffDelay(ui->doubleSpinBox_OffDelay->value());
    rp_control->setLaserOnDelay(ui->doubleSpinBox_OnDelay->value());
    rp_control->setOveride(ui->doubleSpinBox_override->value());

    rp_control->setMoveVelValue(ui->doubleSpinBox_moveVel->value());
    rp_control->setMoveAccValue(ui->doubleSpinBox_moveAcc->value());
    rp_control->setOutlineVelValue(ui->doubleSpinBox_outlineVel->value());
    rp_control->setFillingVelValue(ui->doubleSpinBox_fillingVel->value());
    rp_control->setOutlineFillingAccValue(ui->doubleSpinBox_outlinefillingAcc->value());
}

void WTabControl::axesMoving(bool isforward)
{
    int index = ui->comboBox_axes->currentIndex();

    if(index < 0)
         return;

    double pulse = ui->doubleSpinBox_pulse->value();

    double axes[3] = {0,0,0};

    if(!isforward)
    {
        pulse = -pulse;
    }

    axes[index] = pulse;

//    for(int i=0; i<3; i++)
//    {
//        axes[i] = pulse;
//    }

    setCursor(Qt::WaitCursor);
    //rp_control->moveXYZ2Zero(axes[0],axes[1],axes[2]);
    rp_control->moveXYZ(axes[0],axes[1],axes[2]);
    setCursor(Qt::ArrowCursor);
}

void WTabControl::on_push_forward_clicked()
{
    axesMoving(true);
}

void WTabControl::on_push_backward_clicked()
{
   axesMoving(false);
}

void WTabControl::enableAxesEnable()
{
    ui->checkBox_axesEnable->setChecked(true);
    on_checkBox_axesEnable_clicked(true);
}

void WTabControl::on_checkBox_axesEnable_clicked(bool checked)
{
    if(checked)
    {
        if(rp_control->enableAxis())
        {
            ui->push_forward->setEnabled(true);
            ui->push_backward->setEnabled(true);
        }
    }
    else
    {
        if(rp_control->disableAxis())
        {
            ui->push_forward->setEnabled(false);
            ui->push_backward->setEnabled(false);
        }
    }
    showStatus();
}

void WTabControl::on_checkBox_showSteer_clicked(bool checked)
{ 
    emit showSteer(checked);
}

void WTabControl::setShowSteer(bool checked)
{
     ui->checkBox_showSteer->setChecked(checked);
     emit showSteer(checked);
}

void WTabControl::on_pushButton_clearLimit_clicked()
{
     rp_control->clearLimit();
     showStatus();
}

void WTabControl::on_pushButton_getAxesStatus_clicked()
{
     showStatus();
}

void WTabControl::showStatus()
{
    QImage image;
    VStatus vstatus;
    vstatus.resize(3);
    vstatus[0].resize(3);
    vstatus[1].resize(3);
    vstatus[2].resize(3);

    rp_control->getStatus(vstatus);

    typedef vector<QLabel*> VLABLE;
    vector<VLABLE> xyzlable;

    xyzlable.resize(3);

    xyzlable[0].push_back(ui->lb_Xenable);
    xyzlable[0].push_back(ui->lb_Xp_limit);
    xyzlable[0].push_back(ui->lb_Xn_limit);

    xyzlable[1].push_back(ui->lb_Yenable);
    xyzlable[1].push_back(ui->lb_Yp_limit);
    xyzlable[1].push_back(ui->lb_Yn_limit);

    xyzlable[2].push_back(ui->lb_Zenable);
    xyzlable[2].push_back(ui->lb_Zp_limit);
    xyzlable[2].push_back(ui->lb_Zn_limit);

    for(int i = 0; i < 3; i++)
    {
        if(vstatus[i][0])
            xyzlable[i][0]->setEnabled(true);
        else
            xyzlable[i][0]->setEnabled(false);

        if(vstatus[i][1])
            image.load(":/red_cir.png");
        else
            image.load(":/green_cir.png");

        xyzlable[i][1]->setPixmap(QPixmap::fromImage(image));

        if(vstatus[i][2])
            image.load(":/red_cir.png");
        else
            image.load(":/green_cir.png");

        xyzlable[i][2]->setPixmap(QPixmap::fromImage(image));
    }

}

void WTabControl::on_pushButton_getLaserStatus_clicked()
{
    bool ishigh;
    ishigh = rp_control->getLaserStatus();
    QImage image;

    if(ishigh)
    {
          image.load(":/high.png");
    }
    else
    {
          image.load(":/low.png");
    }
    ui->lb_laserStatus->setPixmap(QPixmap::fromImage(image));
}

void WTabControl::getLaserStatus()
{
    on_pushButton_getLaserStatus_clicked();
}

void WTabControl::on_pushButton_goHome_clicked()
{
    rp_control->goHome();
}

void WTabControl::on_pushButton_curHome_clicked()
{
    rp_control->resetZero();
}

void WTabControl::on_comboBox_singelTrack_currentIndexChanged(int index)
{
    rp_control->setSingleTrack(index);
}

void WTabControl::on_spinBox_MaxLineHeight_valueChanged(int arg1)
{
    rp_control->setSingleHeight(arg1);
}

void WTabControl::on_doubleSpinBox_LengthRadius_valueChanged(double arg1)
{
    rp_control->setSingleLenghtRadius(arg1);
}

void WTabControl::on_doubleSpinBox_deltaZ_valueChanged(double arg1)
{
    rp_control->SetSingleDeltaZ(arg1);
}

void WTabControl::on_doubleSpinBox_OffDelay_valueChanged(double arg1)
{
    rp_control->setLaserOffDelay(arg1);
}

void WTabControl::on_doubleSpinBox_OnDelay_valueChanged(double arg1)
{
    rp_control->setLaserOnDelay(arg1);
}

void WTabControl::on_pushButton_runtimeSetting_clicked()
{
    rp_control->setParaminRuntime();
}

void WTabControl::on_doubleSpinBox_override_valueChanged(double arg1)
{
    rp_control->setOveride(arg1);
}

void WTabControl::on_comboBox_gohomeAxis_currentIndexChanged(int index)
{
    switch(index)
    {
        case 1:
          rp_control->setHomeAxis(1);
        break;

        case 2:
          rp_control->setHomeAxis(2);
        break;

        case 3:
          rp_control->setHomeAxis(4);
        break;

        default:
          rp_control->setHomeAxis(7);
        break;
    }

}

void WTabControl::on_radioButton_LaserPulse_clicked(bool checked)
{
    if(checked)
    {
        if(!interfacePWM)
        {
            //delete other widget at same vacation.
            if(interfaceDA)
            {
                delete interfaceDA;
                interfaceDA = NULL;
            }

            rp_control->setLaserPowerMode(BuildRP::LPMODE_PWM);
            interfacePWM = new PulseLaser(rp_control,this);
            ui->horizontalLayout_powerModeVacancy->addWidget(interfacePWM);
        }
    }
}

void WTabControl::on_radioButton_LaserAnalog_clicked(bool checked)
{
    if(checked)
    {
        if(!interfaceDA)
        {
            if(interfacePWM)
            {
                delete interfacePWM;
                interfacePWM = NULL;
            }

            rp_control->setLaserPowerMode(BuildRP::LPMODE_DA);
            interfaceDA = new AnalogLaser(rp_control,this);
            ui->horizontalLayout_powerModeVacancy->addWidget(interfaceDA);
        }
    }
}

void WTabControl::on_pushButton_originStart_clicked()
{
    rp_control->goOrigin();
    ui->pushButton_originRecord->setEnabled(true);
}



void WTabControl::on_pushButton_originRecord_clicked()
{
    rp_control->setOrigin();
    ui->pushButton_originRecord->setEnabled(false);
}

void WTabControl::attachWidget(QLayout *layout)
{
    ui->verticalLayout_areaContents->addLayout(layout);
}


void WTabControl::on_pushButton_move2Part_clicked()
{
    rp_control->move2Start();
}

void WTabControl::on_pushButton_resetZ_clicked()
{
    rp_control->resetZ();
}

void WTabControl::on_pushButton_reset2BeginPos_clicked()
{
    rp_control->resetZ2Bigin();
}

void WTabControl::on_doubleSpinBox_moveVel_valueChanged(double arg1)
{
    rp_control->setMoveVelValue(arg1);
}

void WTabControl::on_doubleSpinBox_moveAcc_valueChanged(double arg1)
{
    rp_control->setMoveAccValue(arg1);
}

void WTabControl::on_doubleSpinBox_fillingVel_valueChanged(double arg1)
{
    rp_control->setFillingVelValue(arg1);
}

void WTabControl::on_doubleSpinBox_outlineVel_valueChanged(double arg1)
{
     rp_control->setOutlineVelValue(arg1);
}

void WTabControl::on_doubleSpinBox_outlinefillingAcc_valueChanged(double arg1)
{
    rp_control->setOutlineFillingAccValue(arg1);
}

void WTabControl::on_pushButton_gotoPos_clicked()
{
    double tx = rp_control->mm2pulse(ui->doubleSpinBox_goX->value());
    double ty = rp_control->mm2pulse(ui->doubleSpinBox_goY->value());
    double tz = rp_control->mm2pulse(ui->doubleSpinBox_goZ->value());
    rp_control->moveXYZfromZero(tx,ty,tz);
}

void WTabControl::hideAllMoveControl(bool bhide)
{
    setShowSteer(!bhide);
    ui->pushButton_gotoPos->setDisabled(bhide);
    ui->push_forward->setDisabled(bhide);
    ui->push_backward->setDisabled(bhide);
}
