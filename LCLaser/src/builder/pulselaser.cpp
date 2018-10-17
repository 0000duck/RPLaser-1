/***************************************************************************
                          pulselaser.cpp  -  description
                             -------------------
    begin                : Nov. 16 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "pulselaser.h"
#include "ui_pulselaser.h"
#include "config.h"

PulseLaser::PulseLaser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PulseLaser)
{
    ui->setupUi(this);
}

PulseLaser::PulseLaser(BuildRP::RPControl *pctl,QWidget *parent):
    QWidget(parent), ui(new Ui::PulseLaser),
    rp_control(pctl)
{
    ui->setupUi(this);
    double dutyratio = RPConfig::content.getLaserPowerDutyRatio();
    ui->doubleSpinBox_dutyRatio->setValue(dutyratio);
    rp_control->setDutyratio(dutyratio);
}


PulseLaser::~PulseLaser()
{
    delete ui;
}

void PulseLaser::on_doubleSpinBox_frequency_valueChanged(double arg1)
{
    rp_control->setFrequency(arg1);
}

void PulseLaser::on_doubleSpinBox_dutyRatio_valueChanged(double arg1)
{
    rp_control->setDutyratio(arg1);
}
