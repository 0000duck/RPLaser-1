/***************************************************************************
                          pulselaser.h  -  description
                             -------------------
    begin                : Nov. 16 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef PULSELASER_H
#define PULSELASER_H

#include <QWidget>
#include <control.h>

namespace Ui {
class PulseLaser;
}

class PulseLaser : public QWidget
{
    Q_OBJECT

public:
    explicit PulseLaser(QWidget *parent = 0);
    PulseLaser(BuildRP::RPControl *pctl,QWidget *parent = 0);
    ~PulseLaser();

private slots:
    void on_doubleSpinBox_frequency_valueChanged(double arg1);

    void on_doubleSpinBox_dutyRatio_valueChanged(double arg1);

private:
    Ui::PulseLaser *ui;
    BuildRP::RPControl *rp_control;
};

#endif // PULSELASER_H
