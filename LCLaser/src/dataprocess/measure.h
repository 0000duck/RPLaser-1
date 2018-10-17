/***************************************************************************
                          measure.h  -  description
                             -------------------
    begin                : Dec. 8 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef MEASURE_H
#define MEASURE_H

#include <vector>
#include <map>
#include "matrix4x4.h"

class WTabGL;
class MeasureAlignBtn;

typedef std::map<int,Vec4D> SampleMap;//map
typedef std::vector<Vec4D>  SampleList;
typedef std::map<int,double> HeightMap;

class Measure
{
public:
    friend class WTabGL;
    friend class MeasureAlignBtn;

public:
    Measure();
    virtual ~Measure();
    Matrix4X4 getTransMat();
    void clear();

    bool saveTransMat(QString fname);

    bool readLocationSamples(QString fname);

    bool readMeasureSamples(QString fname);
    bool saveMeasureSamples(QString fname);

    bool readPrintSamples(QString fname);
    bool savePrintSamples(QString fname);

    bool readHeight(QString fname);

    bool getTransMeasurePos(Vec3D &pos,int skey);

    Matrix4X4 leastSquareforMat();
    Matrix4X4 calTransMat();

    SampleMap* getPrintSamples();
    SampleMap* getMeasureSamples();
    HeightMap* getHeightSamples();
    SampleList* getLocationSamples();

protected:
    bool readLocationSamples(QString fname, SampleList &samples);
    bool readSamples(QString fname, SampleMap &samples);
    bool saveSamples(QString fname, const SampleMap &samples);
    bool readHeight(QString fname,HeightMap &heightmap);
    Matrix4X4 leastSquareforMat(SampleMap &sourceSamples,SampleMap &targetSamples);
    Matrix4X4 calTransMat(SampleMap &sourceSamples,SampleMap &targetSamples);


private:
     Matrix4X4 m_transMat;
     SampleMap m_measureSamples;
     SampleMap m_printSamples;
     HeightMap m_heightSamples;
     SampleList m_locationSamples;
};


#endif // MEASURE_H
