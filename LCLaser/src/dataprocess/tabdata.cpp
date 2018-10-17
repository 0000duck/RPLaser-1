/***************************************************************************
                          tabdata.cpp  -  description
                             -------------------
    begin                : Apr. 2nd 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "tabdata.h"
#include "ui_tabdata.h"
#include "QMessageBox"
#include "config.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

WTabData::WTabData(DataRP::RPData *rpData, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WTabData),
    rp_data(NULL),
    reflashA(false),
    b_linkInterval2compenRad(false)
{
    ui->setupUi(this);
    ui->groupBox_ballAlogrithm->hide();
    m_compenRadius = RPConfig::content.getLightSpotRadius();
    ui->doubleSpinBox_compenRadius->setValue(m_compenRadius);

    if(b_linkInterval2compenRad)
    {
        setInterval();
    }
    else
    {
         m_pathInterval = RPConfig::content.getPathInterval();
         ui->doubleSpinBox_pathInterval->setValue(m_pathInterval);
    }

    setData(rpData);
}

WTabData::~WTabData()
{
    delete ui;
}

void WTabData::setData(DataRP::RPData *rpData)
{
    rp_data = rpData;
    rp_data->setui(this);

    if(rp_data == NULL)
    {
        QMessageBox::critical(0,QObject::tr("NO data"), QObject::tr("data pointer is NULL"));
        return;
    }

    rp_data->setCutplaneRot(ui->doubleSpinBox_RZPlane->value());

    rp_data->setCompenRadius(ui->doubleSpinBox_compenRadius->value());
    rp_data->setPathInterval(ui->doubleSpinBox_pathInterval->value());

    rp_data->setNumberOutlines(ui->spinBox_numberOutlines->value());
    rp_data->setFittingTrack(ui->checkBox_fitting->isChecked());

    rp_data->switchLaser(false,1);
    ui->checkBox_showanimation->setChecked(rp_data->isDrawProcess());
}

void  WTabData::resetInfo()
{
    QString strMaxLevel = ui->label_maxLevel->text();
    QString strMaxLayer = ui->label_maxLayer->text();
    QString strMinLayer = ui->label_minLayer->text();

    strMaxLevel = strMaxLevel.split(":").at(0);
    strMaxLayer = strMaxLayer.split(":").at(0);
    strMinLayer = strMinLayer.split(":").at(0);

    strMaxLevel.append(": ");
    strMaxLayer.append(": ");
    strMinLayer.append(": ");

    strMaxLevel.append(QString::number(rp_data->getMaxLevel()-1,10));
    strMaxLayer.append(QString::number(rp_data->getMaxLayer(),'.',2));
    strMinLayer.append(QString::number(rp_data->getMinLayer(),'.',2));

    ui->label_maxLevel->setText(strMaxLevel);
    ui->label_maxLayer->setText(strMaxLayer);
    ui->label_minLayer->setText(strMinLayer);
}

void WTabData::slcImported()
{
    rz_obj = 0;
    rz_plane = 110;
    ui->doubleSpinBox_RZOB->setValue(rz_obj);
    ui->doubleSpinBox_RZPlane->setValue(rz_plane);
    resetInfo();
    ui->spinBox_layer->setValue(rp_data->getCurrentLayer());
    emit activateTab(this);
    ui->spinBox_layer->setValue(rp_data->getCurrentLayer());
    ui->spinBox_layer->setMaximum(rp_data->getMaxLevel()-1);
    ui->Slider_layer->setSliderPosition(0);//reset zero.
    ui->spinBox_numberOutlines->setValue(0);
    ui->doubleSpinBox_Zheight->setValue(rp_data->getLayerZ());

    ui->spinBox_toNumbLayer->setMaximum(rp_data->getMaxLevel());
    ui->spinBox_toNumbLayer->setValue(rp_data->getMaxLevel());
}

void WTabData::shwCommdsSize(unsigned csize)
{
    unsigned shwsize;
    QString StrComdSize = ui->label_comdSize->text();
    StrComdSize = StrComdSize.split(":").at(0);
    StrComdSize.append(":  ");
    StrComdSize.append(QString::number(csize,10));
    ui->label_comdSize->setText(StrComdSize);

    if(csize == 0 )
        shwsize = 0;
    else
        shwsize = csize-1;

    ui->spinBox_comdPos->setMaximum(shwsize);
}

void WTabData::shwCurrentCommdPos(unsigned spos)
{
    QString comdPos = ui->label_currComdPos->text();
    comdPos = comdPos.split(":").at(0);
    comdPos.append(":  ");
    comdPos.append(QString::number(spos,10));
    ui->label_currComdPos->setText(comdPos);
    ui->spinBox_comdPos->setValue(spos);
}

void WTabData::on_Slider_layer_sliderMoved(int position)
{
    reflashA = true;

    int currentLayer;

    currentLayer = rp_data->getMaxLevel()* position / (ui->Slider_layer->maximum()-ui->Slider_layer->minimum());

    if(position >ui->Slider_layer->maximum()-1)
    {

        currentLayer = rp_data->getMaxLevel()-1;
    }

//    if( currentLayer > rp_data->getMaxLevel()-1)
//        return;

    rp_data->setCurrentLayer(currentLayer);
    ui->spinBox_layer->setValue(rp_data->getCurrentLayer());

    reflashA = false;
    //ui->spinBox_numberOutlines->setValue(rp_data->getNumberOutlines()); //update number of outlines.
}

void WTabData::animationSlideLayer()
{
    for(int i = 100; i > 1; --i)
    {
#ifdef WIN32
           Sleep(100);
#else
           usleep(100000);
#endif
        on_Slider_layer_sliderMoved(i);
    }
}

void WTabData::on_spinBox_layer_valueChanged(int arg1)
{

    if(reflashA || arg1 > (int)rp_data->getMaxLevel()-1)
       return;

    int position;

     position = (ui->Slider_layer->maximum()-ui->Slider_layer->minimum())*arg1 / rp_data->getMaxLevel();
     ui->Slider_layer->setSliderPosition(position);
     rp_data->setCurrentLayer(arg1);
     //ui->spinBox_numberOutlines->setValue(rp_data->getNumberOutlines());
}

void WTabData::on_radioButton_isAllLayer_clicked(bool checked)
{
      rp_data->setShowAll(checked);
}

void WTabData::on_checkBox_showcut_clicked(bool checked)
{
     rp_data->setShowCut(checked);
}

void WTabData::on_doubleSpinBox_RZPlane_valueChanged(double arg1)
{
    double delta = arg1 - rz_plane;
    rp_data->applyCutPlaneRot(delta);
    rz_plane = arg1;
    rp_data->setCutplaneRot(arg1);
}

void WTabData::on_doubleSpinBox_RZOB_valueChanged(double arg1)
{
    double delta = arg1 - rz_obj;
    rp_data->applyRotateZ(delta);
    rz_obj = arg1;
}

void WTabData::setCompenRadius()
{
    //ui->doubleSpinBox_compenRadius->setMinimum(m_interval/2);
    //ui->doubleSpinBox_compenRadius->setMaximum(m_interval);
    //m_compenRadius =m_interval*5/8;  //m_interval/2(1+1/4);
    ui->doubleSpinBox_compenRadius->setValue(m_compenRadius);
}

void WTabData::setInterval()
{
    ui->doubleSpinBox_pathInterval->setMinimum(m_compenRadius);
    ui->doubleSpinBox_pathInterval->setMaximum(2*m_compenRadius);
    m_interval = m_compenRadius*1.6;
    ui->doubleSpinBox_pathInterval->setValue(m_interval);
}

void WTabData::on_doubleSpinBox_compenRadius_valueChanged(double arg1)
{
    if(rp_data!=NULL)
    {
        m_compenRadius = arg1;
        rp_data->setCompenRadius(arg1);

        if(b_linkInterval2compenRad)
            setInterval();
    }
}

void WTabData::on_doubleSpinBox_pathInterval_valueChanged(double arg1)
{
    if(rp_data!=NULL)
    {
        m_interval = arg1;
        rp_data->setPathInterval(arg1);
        //setCompenRadius();
    }
}

void WTabData::on_spinBox_numberOutlines_valueChanged(int arg1)
{
    if(rp_data != NULL)
    {
        rp_data->setNumberOutlines(arg1);
        rp_data->genPathsandOutlines(false,false);
    }
}

void  WTabData::setOutlineNumber(int numb)
{
    ui->spinBox_numberOutlines->setValue(numb);
    rp_data->setNumberOutlines(numb);
}

void WTabData::on_pushButton_genPaths_clicked()
{

    rp_data->setNumberOutlines(ui->spinBox_numberOutlines->value());
    rp_data->genPathsandOutlines(ui->checkBox_hasPath->isChecked(),ui->checkBox_allLayers->isChecked());
    rp_data->setBar(true,DataRP::BAR_PROCESS);
}

void WTabData::on_pushButton_dataProc_clicked()
{
    rp_data->stepOn();
}

//!show balls animation.
void WTabData::on_checkBox_showanimation_clicked(bool checked)
{
    rp_data->setDrawProcess(checked);
}

void WTabData::on_spinBox_toNumbLayer_valueChanged(int arg1)
{
    rp_data->settoNumbLayer(arg1);
}

void WTabData::on_pushButton_gotoCommd_clicked()
{
    unsigned comdpos = ui->spinBox_comdPos->value();
    emit updateCommdsPos(comdpos);
}

void WTabData::on_pushButton_setZHeight_clicked()
{
    double heightZ = ui->doubleSpinBox_Zheight->value();
    rp_data->setLayerZ(heightZ);
}

void WTabData::on_checkBox_fitting_clicked(bool checked)
{
    rp_data->setFittingTrack(checked);
}

void WTabData::on_pushButton_stopgen_clicked()
{
    rp_data->setStopGenPath(true);
}

void WTabData::showAlogrithm(bool bs)
{
    if(bs)
    {
        ui->groupBox_ballAlogrithm->show();
        rp_data->setDrawProcess(true);
    }
    else
    {
        ui->groupBox_ballAlogrithm->hide();
        rp_data->setDrawProcess(false);
    }
}


void WTabData::on_checkBox_showModel_clicked(bool checked)
{
    rp_data->setShowMesh(checked);
}
