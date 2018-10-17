/***************************************************************************
                          MeasureAlignBtn.cpp  -  description
                             -------------------
    begin                : Dec. 8 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QMessageBox>
#include "control.h"
#include "ui_measureAlignBtn.h"
#include "tabgl.h"
#include "measureAlignBtn.h"
#include "measure.h"

MeasureAlignBtn::MeasureAlignBtn(WTabGL *parent,int skey, int sindex,int heightIndex,bool blocBtn ,int btnIndex):
    m_parent(parent),
    ui(new Ui::MeasureAlignBtn),m_key(skey),
    m_index(sindex),m_heightIndex(heightIndex),
    m_genBtnCnt(0),
    b_locBtn(blocBtn),
    m_btnIndex(btnIndex)
{
    ui->setupUi(this);
    ui->spinBox_index->setMinimum(-1);
    ui->spinBox_index->setValue(m_index);
    ui->spinBox_heightIndex->setValue(m_heightIndex);

    if(b_locBtn)
    {
        ui->pushButton_delete->setDisabled(true);
    }
}

MeasureAlignBtn::~MeasureAlignBtn()
{
    delete ui;
}

void MeasureAlignBtn::on_pushButton_set_clicked()
{
    if(b_locBtn)
    {
        localSet();
    }
    else
    {
        normalSet();
    }
}

void MeasureAlignBtn::on_pushButton_delete_clicked()
{
    SampleMap::iterator sIt;
    sIt = m_parent->m_measure->m_printSamples.find(m_index);
    if(sIt != m_parent->m_measure->m_printSamples.end())
    {
        m_parent->m_measure->m_printSamples.erase(sIt);
    }
    m_parent->removeMeasureLocBtns(m_key);
}

void MeasureAlignBtn::normalSet()
{
    if(!ui->spinBox_index->isEnabled())
    {
        ui->spinBox_index->setEnabled(true);
        ui->spinBox_heightIndex->setEnabled(true);
        return;
    }

    SampleMap::iterator sIt;
    HeightMap::iterator hIt;
    int oldindex = m_index;
    double curZ = 0;
    m_index = ui->spinBox_index->value();
    m_heightIndex = ui->spinBox_heightIndex->value();

    sIt = m_parent->m_measure->m_measureSamples.find(m_index);
    if(sIt == m_parent->m_measure->m_measureSamples.end())
    {
        QMessageBox::critical(0,QObject::tr("Measure Register"), QObject::tr("the index does not exist in measure data"));
        return;
    }

    hIt = m_parent->m_measure->m_heightSamples.find(m_heightIndex);
    if(hIt == m_parent->m_measure->m_heightSamples.end())
    {
        QMessageBox::critical(0,QObject::tr("Measure Register"), QObject::tr("the index does not exist in height data"));
        return;
    }
    else
    {
        curZ = hIt->second;
    }

    //get print postion.
    Vec4D pos;
    m_parent->rp_ctl->getCurrentPosition(pos);
    pos.setValue(2,curZ);

    sIt = m_parent->m_measure->m_printSamples.find(m_index);
    if(sIt != m_parent->m_measure->m_printSamples.end())
    {
        if(m_index != oldindex)
        {
            QMessageBox::critical(0,QObject::tr("Measure Register"), QObject::tr("already have same location"));
            return;
        }
        else //modify with same index.
        {
            sIt->second = pos;
            ui->spinBox_index->setEnabled(false);
            ui->spinBox_heightIndex->setEnabled(false);
            m_parent->setTransMat();
            m_parent->rp_ctl->updateDisplay();
            return;
        }
    }

    sIt = m_parent->m_measure->m_printSamples.find(oldindex);
    if(sIt != m_parent->m_measure->m_printSamples.end())
    {
        m_parent->m_measure->m_printSamples.erase(sIt);
    }

    m_parent->m_measure->m_printSamples.insert(pair<int,Vec4D>(m_index,pos));
    ui->spinBox_index->setEnabled(false);
    ui->spinBox_heightIndex->setEnabled(false);
    m_parent->rp_render->genMarkTex(m_index);
    m_parent->setTransMat();
    m_parent->rp_ctl->updateDisplay();

    if(m_genBtnCnt < 1)
    {
        if((unsigned)m_parent->m_measurelocBtns.size() < m_parent->m_measure->m_measureSamples.size())
        {
            m_parent->addMeasureLocBtns();
            m_genBtnCnt++;
        }
    }
}

void MeasureAlignBtn::localSet()
{
    if(!ui->spinBox_index->isEnabled())
    {
        ui->spinBox_index->setEnabled(true);
        ui->spinBox_heightIndex->setEnabled(true);
        return;
    }

    SampleMap::iterator sIt;
    HeightMap::iterator hIt;
    int oldindex = m_index;
    double curZ = 0;
    m_index = ui->spinBox_index->value();
    m_heightIndex = ui->spinBox_heightIndex->value();

    sIt = m_parent->m_measure->m_measureSamples.find(m_index);
    if(sIt == m_parent->m_measure->m_measureSamples.end())
    {
        QMessageBox::critical(0,QObject::tr("Measure Register"), QObject::tr("the index does not exist in measure data"));
        return;
    }

    hIt = m_parent->m_measure->m_heightSamples.find(m_heightIndex);
    if(hIt == m_parent->m_measure->m_heightSamples.end())
    {
        QMessageBox::critical(0,QObject::tr("Measure Register"), QObject::tr("the index does not exist in height data"));
        return;
    }
    else
    {
        curZ = hIt->second;
    }

    //get print postion.
    Vec4D pos;
    int msize =  (int)m_parent->m_measure->m_locationSamples.size();
    if(m_btnIndex < 0 || m_btnIndex > msize-1 )
        return;
    pos = m_parent->m_measure->m_locationSamples[m_btnIndex];
    pos.setValue(2,curZ);

    sIt = m_parent->m_measure->m_printSamples.find(m_index);
    if(sIt != m_parent->m_measure->m_printSamples.end())
    {
        if(m_index != oldindex)
        {
            QMessageBox::critical(0,QObject::tr("Measure Register"), QObject::tr("already have same location"));
            return;
        }
        else //modify with same index.
        {
            sIt->second = pos;
            ui->spinBox_index->setEnabled(false);
            ui->spinBox_heightIndex->setEnabled(false);
            m_parent->setTransMat();
            m_parent->rp_ctl->updateDisplay();
            return;
        }
    }

    sIt = m_parent->m_measure->m_printSamples.find(oldindex);
    if(sIt != m_parent->m_measure->m_printSamples.end())
    {
        m_parent->m_measure->m_printSamples.erase(sIt);
    }

    m_parent->m_measure->m_printSamples.insert(pair<int,Vec4D>(m_index,pos));
    ui->spinBox_index->setEnabled(false);
    ui->spinBox_heightIndex->setEnabled(false);
    m_parent->rp_render->genMarkTex(m_index);
    m_parent->setTransMat();
    m_parent->rp_ctl->updateDisplay();
}

void MeasureAlignBtn::setChecked(bool bc)
{
     ui->spinBox_index->setEnabled(!bc);
     ui->spinBox_heightIndex->setEnabled(!bc);
     ui->pushButton_set->setChecked(bc);

}
