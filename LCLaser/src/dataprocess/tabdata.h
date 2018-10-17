/***************************************************************************
                          tabdata.cpp  -  description
                             -------------------
    begin                : Apr. 2nd 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DATATAB_H
#define DATATAB_H

#include <QWidget>
#include <QScrollArea>
#include "data.h"

namespace Ui {
class WTabData;
}

class WTabData : public QWidget
{
    Q_OBJECT

public:
    friend class DataRP::RPData;

public:

    explicit WTabData(DataRP::RPData *rpData, QWidget *parent = 0);
    ~WTabData();
    void slcImported();
    void showAlogrithm(bool bs);
    void resetInfo();
    void animationSlideLayer();
    void setOutlineNumber(int numb);

signals:

    void activateTab(QWidget *widget);
    void updateCommdsPos(unsigned pos);

private slots:

    void on_Slider_layer_sliderMoved(int position);

    void on_spinBox_layer_valueChanged(int arg1);

    void on_radioButton_isAllLayer_clicked(bool checked);

    void on_doubleSpinBox_RZOB_valueChanged(double arg1);

    void on_checkBox_showcut_clicked(bool checked);

    void on_doubleSpinBox_RZPlane_valueChanged(double arg1);

    void on_pushButton_genPaths_clicked();

    void on_doubleSpinBox_pathInterval_valueChanged(double arg1);

    void on_doubleSpinBox_compenRadius_valueChanged(double arg1);

    void on_spinBox_numberOutlines_valueChanged(int arg1);

    void on_pushButton_dataProc_clicked();

    void on_checkBox_showanimation_clicked(bool checked);

    void on_spinBox_toNumbLayer_valueChanged(int arg1);

    void shwCommdsSize(unsigned csize);

    void shwCurrentCommdPos(unsigned spos);

    void on_pushButton_gotoCommd_clicked();

    void on_pushButton_setZHeight_clicked();

    void on_checkBox_fitting_clicked(bool checked);

    void on_pushButton_stopgen_clicked();

    void on_checkBox_showModel_clicked(bool checked);

protected:
    void setData(DataRP::RPData *RPData);
    void setCompenRadius();
    void setInterval();

private:
    Ui::WTabData *ui;
    DataRP::RPData *rp_data;
    bool reflashA;
    double rz_obj;
    double rz_plane;
    double m_interval;
    double m_compenRadius;
    double m_pathInterval;

    bool b_linkInterval2compenRad;
};

#endif // DATATAB_H
