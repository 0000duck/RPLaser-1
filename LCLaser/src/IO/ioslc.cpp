/***************************************************************************
                             iOslc.cpp  -  description
                             -------------------
    begin                : Apr. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <QFileDialog>
#include <QMessageBox>
#include "QDebug"
#include "ioslc.h"
#include "database.h"
#include "Vec3D.h"

#ifdef WIN32
#include <io.h>
#endif

#ifdef Linux
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace std;
using namespace IORP;

RPIOslc::RPIOslc():m_current(0)
{
}

RPIOslc::~RPIOslc()
{
}

QString RPIOslc::readSlcHeaderItem(QString keyword, QByteArray &line)
{
    QStringList keywords = DataRP::HeadSlc::keywords.split(",");
    QByteArray item;
    QStringList::iterator it;
    int minIndex,nextindex,index1,current;

    index1 = line.indexOf(keyword,0);
    if(index1 == -1)
         qDebug()<<"can not find keyword :"<<keyword<<endl;

    current = index1 + keyword.length();
    index1 = current;
    minIndex = line.size()-1;

    while(line.at(current) != 0x0A)
    {
        if(line.at(current) == 0x2D)
        {

            for(it = keywords.begin();it != keywords.end();it++)
            {
                nextindex = line.indexOf((*it),current);

                if(nextindex == -1)
                    continue;

                if(nextindex < minIndex)
                {
                    minIndex = nextindex;
                }
            }
        }
        current++;
    }
    item = line.mid(index1,minIndex-index1);

    QString wanted = QString(item).trimmed();

    return wanted;
}

bool RPIOslc::readSlcHeader()
{
    QByteArray line;
    QString extents;
    QStringList extentsList,itemList;
    QStringList::iterator it;
    short index = 0;
    double value[6];

	QFile mfile(m_filename);

	if(!mfile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		cout << "Open failed." << endl;
		return false;
	}

    //read line
    if(!mfile.atEnd())
     {
        line = mfile.readLine();
     }
	m_current = line.size();

    slc->m_SlcHeader.licVer = readSlcHeaderItem("-SLCVER",line).toDouble();
    slc->m_SlcHeader.unit = readSlcHeaderItem("-UNIT",line);
    slc->m_SlcHeader.type = readSlcHeaderItem("-TYPE",line);
    slc->m_SlcHeader.package = readSlcHeaderItem("-PACKAGE",line);
    extents = readSlcHeaderItem("-EXTENTS",line);

    extentsList = extents.split(" ");

    if(extentsList.size() != 3)
    {
        qDebug()<<"extents are not 3 parts "<<endl;
        return false;
    }

    for(it = extentsList.begin();it!=extentsList.end();it++)
    {
        itemList = it->split(",");
         if(itemList.size() != 2)
         {
             qDebug()<<"extents are not min and max "<<endl;
             return false;
         }
         value[index++] = itemList.at(0).toDouble();
         value[index++] = itemList.at(1).toDouble();
    }
    slc->m_SlcHeader.extents = DataRP::HeadSlc::Extent(value[0],value[1],value[2],value[3],value[4],value[5]);

    slc->m_SlcHeader.debug();

	mfile.close();

    return true;
}

bool RPIOslc::readSlcData()
{
    float maxZlayer;
    float floatbuf[4];
    unsigned buff[2];
    char charbuf[256];
    unsigned filesize=0,scount = 0,byteCount = 0;
    bool b_start = true;

    DataRP::RPslice slice;
    DataRP::RPcontour contour;

    FILE *stream;

    QByteArray ba = m_filename.toLocal8Bit();

    if((stream=fopen(ba.data(),"rb"))==NULL)
    {
        cout<<"Cannotopenoutputfile."<<endl;
        return false;
    }

#ifdef WIN32
    filesize = filelength(fileno(stream));
#endif

#ifdef Linux
    struct stat ST;
    stat(m_filename.toLatin1(),&ST);
    filesize = ST.st_size;
#endif

     filesize -= m_current;

    fseek(stream,m_current,SEEK_SET);
    fread(charbuf,sizeof(char),1,stream);

    while( charbuf[0] != 0X20)
    {
       fread(charbuf,sizeof(char),1,stream);
    }

     fread(charbuf,sizeof(char),255,stream);

    fread(charbuf,sizeof(char),1,stream);
    unsigned entrynumb =( unsigned)charbuf[0];

    //qDebug()<<"entry numb = "<<entrynumb<<endl;

    for(unsigned i = 0; i < entrynumb; i++ )
    {
        fread(floatbuf,sizeof(float),4,stream);
        DataRP::TableSlc table_slc(floatbuf[0],floatbuf[1],floatbuf[2],floatbuf[3]);
        table_slc.debug();
        slc->m_SlcTableEntry.push_back(table_slc);
    }

    while( byteCount < filesize )
    {
        fread(floatbuf,sizeof(float),1,stream);
        maxZlayer = floatbuf[0];
        slice.minZLevel = maxZlayer;
        byteCount++;

        fread(buff,sizeof(unsigned),1,stream);
        slice.numb_outerBoundaries = buff[0];

        if(slice.numb_outerBoundaries ==0xFFFFFFFF)
            break;

        byteCount++;
        //too many boundaries.
        if(slice.numb_outerBoundaries >1000)
        {
            qDebug()<<"read erro "<<endl;
            break;
        }

        Vec3D pos;
        Vec2D point;
        Matrix4X4 transmat;

        for(unsigned i = 0; i < slice.numb_outerBoundaries; i++)
        {
            fread(buff,sizeof(unsigned),2,stream);
            contour.numb_verts = buff[0];
            contour.numb_gaps = buff[1];

            byteCount += contour.numb_verts*2;

              for(unsigned j = 0; j < contour.numb_verts; j++)
              {
                  fread(floatbuf,sizeof(float),2,stream);

                  pos = Vec3D(floatbuf[0],floatbuf[1],0);
                  transmat = slc->getTransMat();
                  pos = transmat * pos;

                  point = pos.toVec2D();
                  contour.vertList.push_back(point);

                  if( b_start )
                  {
                       slc->m_boundingBox.MinMax[0][0] = point.getX();
                       slc->m_boundingBox.MinMax[0][1] = point.getX();
                       slc->m_boundingBox.MinMax[1][0] = point.getY();
                       slc->m_boundingBox.MinMax[1][1] = point.getY();
                       b_start = false;
                  }
                  else
                  {
                      if( point.getX() < slc->m_boundingBox.MinMax[0][0])
                         slc->m_boundingBox.MinMax[0][0] =  point.getX();

                      if( point.getX() > slc->m_boundingBox.MinMax[0][1])
                          slc->m_boundingBox.MinMax[0][1] =  point.getX();

                      if( point.getY() < slc->m_boundingBox.MinMax[1][0])
                          slc->m_boundingBox.MinMax[1][0] =  point.getY();

                      if(point.getY() > slc->m_boundingBox.MinMax[1][1])
                          slc->m_boundingBox.MinMax[1][1] =  point.getY();
                  }
              }

              if(contour.vertList.size() < 3)
              {
                  QMessageBox::critical(0,QObject::tr("boundary too few"), QObject::tr("boundary vertices are too few"));
              }

              slice.outerBoundaries.push_back(contour);
              contour.clear();
        }
        slc->m_SlcSlices.push_back(slice);
        slice.clear();
        scount++;
     }

    slc->setMinLayer(slc->m_SlcSlices.begin()->minZLevel);
    slc->setMaxLayer(maxZlayer);
    slc->setMaxLevel(scount);
    slc->setObjectToZeorZ();
    slc->m_boundingBox.setPeaks();

    fclose(stream);
    return true;
}
void RPIOslc::openFile(QString fileName,DataRP::RPData *sslc)
{
    m_filename = fileName;
    slc = sslc;
    readSlcHeader();
    readSlcData();
}
