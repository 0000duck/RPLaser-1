/***************************************************************************
                          analoglaser.h  -  description
                             -------------------
    begin                : Nov. 16 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef ANALOGLASER_H
#define ANALOGLASER_H

#include <QWidget>
#include <control.h>

namespace Ui {
class AnalogLaser;
}

class AnalogLaser : public QWidget
{
    Q_OBJECT

public:
    explicit AnalogLaser(QWidget *parent = 0);
    AnalogLaser(BuildRP::RPControl *pctl,QWidget *parent = 0);
    ~AnalogLaser();

private slots:
    void on_doubleSpinBox_voltage_valueChanged(double arg1);

private:
    Ui::AnalogLaser *ui;
    BuildRP::RPControl *rp_control;
};

#endif // ANALOGLASER_H
