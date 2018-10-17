/***************************************************************************
                          measure.cpp  -  description
                             -------------------
    begin                : Dec. 8 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QDir>
#include <QMessageBox>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string.h>
#include "measure.h"

Measure::Measure()
{
}

Measure::~Measure()
{
    clear();
}

void Measure::clear()
{
     m_measureSamples.clear();
     m_printSamples.clear();
     m_heightSamples.clear();
     m_locationSamples.clear();
}

bool Measure::saveTransMat(QString fname)
{
    QString fileName= QDir::currentPath() + fname;
    QByteArray filename = fileName.toLocal8Bit();
    ofstream matStream(filename.data(),ios::out);
    if(!matStream.is_open())
    {
        return false;
    }

    Vec4D rows[4];
    m_transMat.getRows(rows);
    for(int i = 0; i < 4; i++)
    {
        matStream<<setprecision(16)<<rows[i].getValue(0)<<" "<<rows[i].getValue(1)<<" "<<rows[i].getValue(2)<<" "<<rows[i].getValue(3)<<endl;
    }
    matStream.close();
    return true;
}

bool Measure::readLocationSamples(QString fname)
{
    m_locationSamples.clear();
    return readLocationSamples(fname,m_locationSamples);
}

bool Measure::readMeasureSamples(QString fname)
{
    m_measureSamples.clear();
    return readSamples(fname,m_measureSamples);
}

bool Measure::saveMeasureSamples(QString fname)
{
    return saveSamples(fname,m_measureSamples);
}

bool Measure::readPrintSamples(QString fname)
{
    m_printSamples.clear();
    return readSamples(fname,m_printSamples);
}

bool Measure::savePrintSamples(QString fname)
{
    return saveSamples(fname,m_printSamples);
}

bool Measure::readHeight(QString fname)
{
    return readHeight(fname,m_heightSamples);
}

Matrix4X4 Measure::leastSquareforMat()
{
    return leastSquareforMat(m_measureSamples,m_printSamples);
}

Matrix4X4 Measure::calTransMat()
{
    return calTransMat(m_measureSamples,m_printSamples);
}

SampleMap* Measure::getPrintSamples()
{
return &m_printSamples;
}

SampleMap* Measure::getMeasureSamples()
{
    return &m_measureSamples;
}

HeightMap* Measure::getHeightSamples()
{
    return &m_heightSamples;
}

SampleList* Measure::getLocationSamples()
{
   return &m_locationSamples;
}

/***************************protected function************************************/
bool Measure::readLocationSamples(QString fname, SampleList &samples)
{
    QString fileName= QDir::currentPath()+fname;
    QByteArray filename = fileName.toLocal8Bit();
    ifstream locationStream(filename.data(),ios::in);
    if(!locationStream.is_open())
    {
        return false;
    }

    char buff[1024];
    double value[]={0,0,0,1};
    Vec4D item;

    while(!locationStream.eof())
    {
        memset(buff,0,sizeof(char)*1024);
        locationStream.getline(buff,1024);

        QString str = QString(QLatin1String(buff));
        str.trimmed();
        if(str == "")
            continue;

        stringstream strStream(buff);
        strStream>>value[0]>>value[1];
        item.setValue(value);
        samples.push_back(item);
    }
    locationStream.close();
    return true;
}

bool Measure::readHeight(QString fname,HeightMap &heightmap)
{
    QString fileName= QDir::currentPath()+fname;
    QByteArray filename = fileName.toLocal8Bit();
    ifstream heightStream(filename.data(),ios::in);
    if(!heightStream.is_open())
    {
        return false;
    }

    char buff[1024];
    int index;
    double value;

    while(!heightStream.eof())
    {
        index = 0; value = 0;
        memset(buff,0,sizeof(char)*1024);
        heightStream.getline(buff,1024);

        QString str = QString(QLatin1String(buff));
        str.trimmed();
        if(str == "")
            continue;

        stringstream strStream(buff);
        strStream>>index>>value;
        heightmap.insert(pair<int,double>(index,value));
    }
    heightStream.close();
    return true;
}

bool Measure::readSamples(QString fname, SampleMap &samples)
{
    QString fileName= QDir::currentPath()+fname;
    QByteArray filename = fileName.toLocal8Bit();
    ifstream sampleStream(filename.data(),ios::in);
    if(!sampleStream.is_open())
    {
        return false;
    }

    char buff[1024];
    double value[]={0,0,0,1};
    int index;
    Vec4D item, emptyItem;
    emptyItem.setValue(value);

    while(!sampleStream.eof())
    {
        index = -1; value[0] = 0; value[1] = 0; value[2] = 0; value[3] = 1;
        memset(buff,0,sizeof(char)*1024);
        sampleStream.getline(buff,1024);
        stringstream strStream(buff);
        strStream>>index>>value[0]>>value[1]>>value[2];
        item.setValue(value);

        if(item != emptyItem)
            samples.insert(pair<int,Vec4D>(index,item));
    }
    sampleStream.close();
    return true;
}

bool Measure::saveSamples(QString fname, const SampleMap &samples)
{
    QString fileName= QDir::currentPath()+fname;
    QByteArray filename = fileName.toLocal8Bit();
    ofstream sampleStream(filename.data(),ios::out);
    if(!sampleStream.is_open())
    {
        return false;
    }

    SampleMap::const_iterator sIt;
    for(sIt = samples.begin(); sIt != samples.end(); sIt++)
    {
        sampleStream<<sIt->first<<" ";
        for(int i = 0; i < 2; i++)
        {
            sampleStream<<setprecision(16)<<sIt->second.getValue(i)<<" ";
        }
        sampleStream<<setprecision(16)<<sIt->second.getValue(2)<<endl;
    }

    sampleStream.close();
    return true;
}

Matrix4X4 Measure::leastSquareforMat(SampleMap &sourceSamples,SampleMap &targetSamples)
{
    SampleList sSamples,tSamples;
    SampleMap::iterator sourceIt,targetIt;
    int key;
    for(sourceIt = sourceSamples.begin(); sourceIt != sourceSamples.end(); sourceIt++ )
    {
        key = sourceIt->first;
        targetIt = targetSamples.find(key);
        if(targetIt == targetSamples.end())
        {
            continue;
        }
        else
        {
            sSamples.push_back(sourceIt->second);
            tSamples.push_back(targetIt->second);
        }
    }

    Matrix4X4  sMat, tMat,rMat;
    Vec4D vecP[4];
    Vec4D vecQ[4];
    double pci;
    Vec4D srow, trow;
    SampleList::iterator sIt;
    unsigned int ssize, i,j;
    double vec[]={0,0,0,0};

    for( i = 0; i < 4; i++)
    {
        vecP[i].setValue(vec);
        vecQ[i].setValue(vec);
    }

    if(sSamples.size() != tSamples.size() ||(sSamples.size() == 0 &&  tSamples.size() == 0))
    {
        qDebug()<<"sample size is not equal"<<endl;
        return rMat;
    }

    ssize = sSamples.size();

    for(sIt = sSamples.begin(); sIt != sSamples.end(); sIt++)
    {
        vecP[3] += (*sIt);
    }

    for(sIt = tSamples.begin(); sIt != tSamples.end(); sIt++)
    {
        vecQ[3] += (*sIt);
    }


    for( i = 0; i < 3; i++)
        for( j = 0; j < ssize; j++)
        {
            pci = sSamples[j].getValue(i);
            srow = sSamples[j];
            trow = tSamples[j];
            vecP[i] +=  srow*pci;
            vecQ[i] +=  trow*pci;
        }

sMat.setValue(vecP);
tMat.setValue(vecQ);


Matrix4X4 sampleMat = sMat.transpose();
Matrix4X4 mappedMat = tMat.transpose();

sampleMat = sampleMat.inverse();
m_transMat = mappedMat * sampleMat;
return m_transMat;
}

Matrix4X4 Measure::calTransMat(SampleMap &sourceSamples,SampleMap &targetSamples)
{
    SampleList sSamples,tSamples;
    SampleMap::iterator sourceIt,targetIt;
    int key;
    int cnt = 0;
    for(sourceIt = sourceSamples.begin(); sourceIt != sourceSamples.end(); sourceIt++ )
    {
        key = sourceIt->first;
        targetIt = targetSamples.find(key);
        if(targetIt == targetSamples.end())
        {
            continue;
        }
        else
        {
            sSamples.push_back(sourceIt->second);
            tSamples.push_back(targetIt->second);
            cnt++;
        }
        if(cnt == 4)
            break;
    }

    Matrix4X4 transMat,sMat,tMat;
    if(!(sSamples.size() == 4 && tSamples.size() == 4))
    {
        qDebug()<<"sample size is not 4"<<endl;
        return transMat;
    }

    for(unsigned i = 0; i < 4; i++)
    {
        sMat.setValue(i,sSamples[i]);
        tMat.setValue(i,tSamples[i]);
    }

    Matrix4X4 sampleMat = sMat.transpose();
    Matrix4X4 mappedMat = tMat.transpose();

    sampleMat = sampleMat.inverse();
    m_transMat = mappedMat * sampleMat;
    return m_transMat;
}

bool Measure::getTransMeasurePos(Vec3D &pos,int skey)
{
    SampleMap::iterator sIt;
    sIt = m_measureSamples.find(skey);
    if(sIt == m_measureSamples.end())
    {
        QMessageBox::critical(0,QObject::tr("mesure Points"),QObject::tr("no this Point index"));
        return false;
    }
    Vec4D transpos = m_transMat * sIt->second;

    pos = transpos.toVec3D();
    return true;
}

