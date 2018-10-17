/***************************************************************************
                          analoglaser.cpp  -  description
                             -------------------
    begin                : Nov. 16 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "analoglaser.h"
#include "ui_analoglaser.h"
#include "config.h"

AnalogLaser::AnalogLaser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalogLaser)
{
    ui->setupUi(this);
}

AnalogLaser::AnalogLaser(BuildRP::RPControl *pctl,QWidget *parent):
    QWidget(parent), ui(new Ui::AnalogLaser),
    rp_control(pctl)
{
    ui->setupUi(this);
    double voltage = RPConfig::content.getLaserPowervoltage();
    ui->doubleSpinBox_voltage->setValue(voltage);
    rp_control->setVoltage(voltage);
}


AnalogLaser::~AnalogLaser()
{
    delete ui;
}

void AnalogLaser::on_doubleSpinBox_voltage_valueChanged(double arg1)
{
    rp_control->setVoltage(arg1);
}
