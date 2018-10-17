/***************************************************************************
                          tabgl.h  -  description
                             -------------------
    begin                : Apr. 2nd 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include "tabgl.h"
#include "ui_tabgl.h"
#include "control.h"
#include "config.h"

WTabGL::WTabGL(RenderRP::RPDisplay *sRenderRP, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WTabGL),
    rp_render(NULL),
    m_sposStatus(0),m_measurelocBtnKey(0)
{
    ui->setupUi(this);
    setRender(sRenderRP);

    m_heightEachLayer = RPConfig::content.getHeightEachLayer();
    ui->doubleSpinBox_deltaZ->setValue(m_heightEachLayer);
    ui->checkBox_compensation->setChecked(RPConfig::content.getCoordcompensation());
    rp_data->setCoordCompensation(RPConfig::content.getCoordcompensation());
    ui->doubleSpinBox_rotZ->setValue(RPConfig::content.getRotZ());
    ui->doubleSpinBox_transX->setValue(RPConfig::content.getTransX());
    ui->doubleSpinBox_transY->setValue(RPConfig::content.getTransY());

    if(!rp_data->isCoordCompensation())
    {
        ui->doubleSpinBox_rotZ->setDisabled(true);
        ui->doubleSpinBox_transX->setDisabled(true);
        ui->doubleSpinBox_transY->setDisabled(true);
    }
}

WTabGL::~WTabGL()
{
    delete ui;
}

void WTabGL::setRender(RenderRP::RPDisplay *sRenderRP)
{
    rp_render = sRenderRP;
    rp_data = rp_render->getdata();
    rp_ctl = rp_data->getControl();
    m_measure = rp_render->getMeasure();

    if(rp_render != NULL)
    {
        rp_render->getGrid(m_gridScale,m_gridWidth,m_gridHeight);
        b_background = rp_render->isShowBackground();
    }
    else
    {
         QMessageBox::critical(0,QObject::tr("NO render"), QObject::tr("render pointer is NULL"));
         return;
    }
    ui->spinBox_gridScale->setValue(m_gridScale);
    rp_render->setShowLaser(ui->checkBox_showLaser->isChecked());
    rp_render->setArrows(ui->checkBox_showArrows->isChecked());
    rp_render->setAABB(ui->checkBox_AABB->isChecked());
    rp_render->setshowWireframe(ui->radioButton_wireframe->isChecked());

    rp_data->setDeltaZ(ui->doubleSpinBox_deltaZ->value());

    ui->checkBox_background->setChecked(b_background);
}

void WTabGL::on_spinBox_gridScale_valueChanged(int arg1)
{
    rp_render->setGridScale(arg1);
    emit updateDisplay();
}

void WTabGL::on_checkBox_showLaser_clicked(bool checked)
{
   rp_render->setShowLaser(checked);
   emit updateDisplay();
}

void WTabGL::setShowLaserHead(bool bshow)
{
    ui->checkBox_showLaser->setChecked(bshow);
    on_checkBox_showLaser_clicked(bshow);
}

void WTabGL::on_checkBox_showArrows_clicked(bool checked)
{
    rp_render->setArrows(checked);
    emit updateDisplay();
}

void WTabGL::on_pushButton_clean_tracks_clicked()
{
   rp_render->clearTracks();
   emit updateDisplay();
}

void WTabGL::on_checkBox_AABB_clicked(bool checked)
{
    rp_render->setAABB(checked);
    emit updateDisplay();
}

void WTabGL::on_checkBox_background_clicked(bool checked)
{
    rp_render->showBackground(checked);
    emit updateDisplay();
}

void WTabGL::on_radioButton_soild_clicked(bool checked)
{
    rp_render->setshowWireframe(!checked);
    emit updateDisplay();
}

void WTabGL::on_radioButton_wireframe_clicked(bool checked)
{
    rp_render->setshowWireframe(checked);
    emit updateDisplay();
}

void WTabGL::on_pushButton_slice_clicked()
{
    slice();
}

void WTabGL::slice()
{
    if(rp_data->meshRenderable())
    {
        rp_data->startSlice();
    }
}

void WTabGL::on_doubleSpinBox_deltaZ_valueChanged(double arg1)
{
    rp_data->setDeltaZ(arg1);
}

void WTabGL::clearMeasureButtons()
{
    MeasureAlignBtns::iterator mIt;
    mIt = m_measurelocBtns.begin();

    while(mIt != m_measurelocBtns.end())
    {
        delete mIt.value();
        mIt = m_measurelocBtns.erase(mIt);
    }
}

void  WTabGL::resetMeasureLocBtns()
{
 clearMeasureButtons();
 SampleMap::iterator pIt,mIt;
 SampleMap *printmap = m_measure->getPrintSamples();
 SampleMap *measuremap = m_measure->getMeasureSamples();

 if(printmap->size() < 1)
     return;

 int key;
 int heightkey = 0;
 double printZ;

 for(pIt = printmap->begin();pIt !=  printmap->end(); pIt++)
 {
    key = pIt->first;
    mIt = measuremap->find(key);
    if(mIt == measuremap->end())
    {
        continue;
    }
    else
    {
        rp_render->genMarkTex(key);

        printZ = pIt->second.getValue(2);
        HeightMap::iterator hIt;
        HeightMap* heightmap = m_measure->getHeightSamples();
        for(hIt = heightmap->begin(); hIt != heightmap->end(); hIt++ )
        {
            if(hIt->second == printZ)
            {
                 heightkey = hIt->first;
                 break;
            }
        }

        MeasureAlignBtn* sbtn = new MeasureAlignBtn(this,m_measurelocBtnKey,key,heightkey);
        sbtn->setChecked(true);
        m_measurelocBtns.insert(m_measurelocBtnKey++,sbtn);
        ui->verticalLayout_buttons->addWidget(sbtn);
    }
 }
 addMeasureLocBtns();
 rp_ctl->updateDisplay();
}

void  WTabGL::addLocationBtns()
{
    clearMeasureButtons();
    SampleList::iterator lIt;
    MeasureAlignBtn* sbtn;
    SampleList *locList = m_measure->getLocationSamples();
    unsigned lsize = locList->size();

    for(unsigned i = 0; i < lsize; i++)
    {
        sbtn = new MeasureAlignBtn(this,m_measurelocBtnKey,-1,0,true,i);
        m_measurelocBtns.insert(m_measurelocBtnKey++,sbtn);
        ui->verticalLayout_buttons->addWidget(sbtn);
    }
}

/********************** measure register ********************************/
 void WTabGL::on_pushButton_readLocation_clicked()
 {
     QString filename = "/../share/location.txt";
     if(!m_measure->readLocationSamples(filename))
     {
          QMessageBox::critical(0,QObject::tr("Location file cannot open!"),filename);
          return;
     }
     addLocationBtns();
 }

void WTabGL::on_pushButton_readMeasurePos_clicked(bool checked)
{
    if(checked)
    {
        QString filename = "/../globaPoints/export.asc";
        if(!m_measure->readMeasureSamples(filename))
        {
             QMessageBox::critical(0,QObject::tr("Measure postion file cannot open!"),filename);
             return;
        }

        filename = "/../globaPoints/height.txt";
        if(!m_measure->readHeight(filename))
        {
             QMessageBox::critical(0,QObject::tr("height file cannot open!"),filename);
             return;
        }

        ui->pushButton_readLocation->setEnabled(true);
        addMeasureLocBtns();
    }
    else
    {
        m_measure->clear();
        clearMeasureButtons();
        rp_data->resetTransMat();
        ui->pushButton_readLocation->setEnabled(false);
    }
}

//!read preset position.
void WTabGL::on_pushButton_ReadPos_clicked()
{
    QString filename = "/../share/ppos.txt";
    if(!m_measure->readPrintSamples(filename))
    {
         QMessageBox::critical(0,QObject::tr("print postion file cannot open!"),filename);
         return;
    }
    resetMeasureLocBtns();
    QMessageBox::information(0,QObject::tr("Measure register"),QObject::tr("read printer mark position OK"));
    setTransMat();
}

void WTabGL::readMeasureAndPos()
{
    on_pushButton_readMeasurePos_clicked(true);
    on_pushButton_ReadPos_clicked();
}

void WTabGL::on_pushButton_savePos_clicked()
{
    QString filename = "/../share/ppos.txt";
    if(!m_measure->savePrintSamples(filename))
    {
         QMessageBox::critical(0,QObject::tr("print postion file cannot save!"),filename);
         return;
    }
    QMessageBox::information(0,QObject::tr("Measure register"),QObject::tr("save printer mark position OK!"));
}

void WTabGL::addMeasureLocBtns()
{
    MeasureAlignBtn* sbtn = new MeasureAlignBtn(this,m_measurelocBtnKey);
    m_measurelocBtns.insert(m_measurelocBtnKey++,sbtn);
    ui->verticalLayout_buttons->addWidget(sbtn);
}

void WTabGL::removeMeasureLocBtns(int key)
{
    MeasureAlignBtns::Iterator mIt;

    mIt = m_measurelocBtns.find(key);
    if(mIt != m_measurelocBtns.end() && m_measurelocBtns.size() > 1 )
    {

        delete mIt.value();
        m_measurelocBtns.erase(mIt);
    }
    rp_ctl->updateDisplay();
}

void WTabGL::setTransMat()
{
     SampleMap *printmap = m_measure->getPrintSamples();

     if(printmap->size() > 3)
       rp_data->setTransMat(m_measure->leastSquareforMat());

     //m_measure->saveTransMat("/../share/transmat.txt");
}

void WTabGL::attachWidget(QLayout *layout)
{
    ui->verticalLayout_areaContents->addLayout(layout);
}

void WTabGL::on_pushButton_vertify_clicked()
{
    int measureSampleIndex = ui->spinBox_measureIndex->value();
    rp_data->vertifyMeasurePos(measureSampleIndex);
}

void WTabGL::on_checkBox_compensation_clicked(bool checked)
{
    rp_data->setCoordCompensation(checked);
    ui->doubleSpinBox_rotZ->setEnabled(checked);
    ui->doubleSpinBox_transX->setEnabled(checked);
    ui->doubleSpinBox_transY->setEnabled(checked);
}


//void WTabGL::getCompensationParam(double &rotZ,double &transX, double &transY)
//{
//    rotZ = ui->doubleSpinBox_rotZ->value();
//    transX = ui->doubleSpinBox_transX->value();
//    transY = ui->doubleSpinBox_transY->value();
//}
