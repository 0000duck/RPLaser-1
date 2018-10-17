/***************************************************************************
                          tabgl.h  -  description
                             -------------------
    begin                : Apr. 2nd 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef GLTAB_H
#define GLTAB_H

#include <QWidget>
#include <QMap>
#include "display.h"
#include "measureAlignBtn.h"

typedef QMap<int,MeasureAlignBtn *> MeasureAlignBtns;

namespace Ui {
class WTabGL;
}


class WTabGL : public QWidget
{
    Q_OBJECT
public:
    friend class MeasureAlignBtn;

public:
    explicit WTabGL(RenderRP::RPDisplay *sRenderRP, QWidget *parent = 0);
    ~WTabGL();

    void attachWidget(QLayout *layout);
    void readMeasureAndPos();
    void slice();
    void setShowLaserHead(bool bshow);
//    void getCompensationParam(double &rotZ, double &transX, double &transY);

signals:

     void updateDisplay();


private slots:
    void on_spinBox_gridScale_valueChanged(int arg1);

    void on_checkBox_showLaser_clicked(bool checked);

    void on_pushButton_clean_tracks_clicked();

    void on_checkBox_showArrows_clicked(bool checked);

    void on_checkBox_AABB_clicked(bool checked);

    void on_checkBox_background_clicked(bool checked);

    void on_radioButton_soild_clicked(bool checked);

    void on_radioButton_wireframe_clicked(bool checked);

    void on_pushButton_slice_clicked();

    void on_doubleSpinBox_deltaZ_valueChanged(double arg1);

    void on_pushButton_readMeasurePos_clicked(bool checked);

    void on_pushButton_ReadPos_clicked();

    void on_pushButton_savePos_clicked();

    void on_pushButton_readLocation_clicked();

    void on_pushButton_vertify_clicked();

    void on_checkBox_compensation_clicked(bool checked);

protected:
    void setRender(RenderRP::RPDisplay *sRenderRP);

    void resetMeasureLocBtns();

    void addLocationBtns();

    void clearMeasureButtons();

    void setTransMat();

protected:
    Matrix4X4 m_samplePos;
    Matrix4X4 m_mappedPos;

private:
    Ui::WTabGL *ui;
    RenderRP::RPDisplay *rp_render;
    DataRP::RPData *rp_data;
    BuildRP::RPControl *rp_ctl;

    int m_gridScale,m_gridWidth,m_gridHeight;
    bool b_background;
    unsigned short m_sposStatus;
    double m_heightEachLayer;
    int m_measurelocBtnKey;
    MeasureAlignBtns m_measurelocBtns;
    Measure *m_measure;

private:
    void addMeasureLocBtns();
    void removeMeasureLocBtns(int key);
};

#endif // GLTAB_H
