/***************************************************************************
                          tabcontrol.h  -  description
                             -------------------
    begin                : Apr. 2nd 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef TABCONTROL_H
#define TABCONTROL_H

#include <QWidget>
#include <QLabel>
#include "control.h"
#include "data.h"
#include "matrix4x4.h"
#include "pulselaser.h"
#include "analoglaser.h"

namespace Ui {
class WTabControl;
}

class WTabControl : public QWidget
{
    Q_OBJECT

public:
    WTabControl(BuildRP::RPControl *RPControl, DataRP::RPData *rpdata,QWidget *parent);
    ~WTabControl();

    void setParam();
    void showStatus();
    void enableAxesEnable();
    void saveConfig();
    void attachWidget(QLayout *layout);
    void setShowSteer(bool checked);
    void hideAllMoveControl(bool bhide);
signals:
    void showSteer(bool bshow);

public slots:
    void getLaserStatus();

protected:
    void setControl(BuildRP::RPControl *RPControl);
    void setData(DataRP::RPData *pdata);
    void axesMoving(bool isforward);


private slots:
    void on_checkBox_axesEnable_clicked(bool checked);

    void on_checkBox_showSteer_clicked(bool checked);

    void on_push_forward_clicked();

    void on_push_backward_clicked();

    void on_pushButton_clearLimit_clicked();

    void on_pushButton_getAxesStatus_clicked();

    void on_pushButton_getLaserStatus_clicked();

    void on_pushButton_goHome_clicked();

    void on_pushButton_curHome_clicked();

    void on_comboBox_singelTrack_currentIndexChanged(int index);

    void on_spinBox_MaxLineHeight_valueChanged(int arg1);

    void on_doubleSpinBox_LengthRadius_valueChanged(double arg1);

    void on_doubleSpinBox_deltaZ_valueChanged(double arg1);

    void on_doubleSpinBox_OffDelay_valueChanged(double arg1);

    void on_doubleSpinBox_OnDelay_valueChanged(double arg1);

    void on_pushButton_runtimeSetting_clicked();

    void on_doubleSpinBox_override_valueChanged(double arg1);

    void on_comboBox_gohomeAxis_currentIndexChanged(int index);

    void on_radioButton_LaserPulse_clicked(bool checked);

    void on_radioButton_LaserAnalog_clicked(bool checked);

    void on_pushButton_originStart_clicked();

    void on_pushButton_originRecord_clicked();

    void on_pushButton_resetZ_clicked();

    void on_pushButton_move2Part_clicked();

    void on_pushButton_reset2BeginPos_clicked();

    void on_doubleSpinBox_moveVel_valueChanged(double arg1);

    void on_doubleSpinBox_moveAcc_valueChanged(double arg1);

    void on_doubleSpinBox_fillingVel_valueChanged(double arg1);

    void on_doubleSpinBox_outlineVel_valueChanged(double arg1);

    void on_doubleSpinBox_outlinefillingAcc_valueChanged(double arg1);

    void on_pushButton_gotoPos_clicked();

private:
    Ui::WTabControl *ui;
    BuildRP::RPControl *rp_control;
    PulseLaser *interfacePWM;
    AnalogLaser *interfaceDA;
};

#endif // TABCONTROL_H
