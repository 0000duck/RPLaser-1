/***************************************************************************
                          database.h  -  description
                             -------------------
    begin                : Apr. 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <QString>
#include <QDebug>
#include <QStringList>
#include "Vec2D.h"
#include "matrix4x4.h"

using namespace std;

namespace DataRP
{
/******************MOdels********************/
    struct RPColor
    {
        RPColor(){}

        RPColor(float red, float green,float blue,float alpha = 0 )
        {
            rgba[0] = red;
            rgba[1] = green;
            rgba[2] = blue;
            rgba[3] = alpha;
        }

        RPColor& operator = (const RPColor &other)
        {
            for(int i = 0; i < 4; i++)
            {
                rgba[i] = other.rgba[i];
            }
            return *this;
        }

        void setRed(float red)
        {
           rgba[0] = red;
        }

        void setGreen(float green)
        {
           rgba[1] = green;
        }

        void setBlue(float blue)
        {
           rgba[2] = blue;
        }

        void setAlpha(float alpha)
        {
           rgba[3] = alpha;
        }

        float red() const
        {
            return rgba[0];
        }

        float green()const
        {
            return rgba[1];
        }

        float blue()const
        {
            return rgba[2];
        }

        float alpha()const
        {
            return rgba[3];
        }


        float rgba[4];
    };//RPColor

    struct RPUV
    {
        double u,v;

        RPUV():u(0.0),v(0.0){}

        RPUV(double su,double sv)
        {
            u = su;
            v = sv;
        }

        RPUV& operator = (const RPUV &other)
        {
           u = other.u;
           v = other.v;
           return *this;
        }

        void  setUV(double su,double sv)
        {
            u = su;
            v = sv;
        }

        void  setU(double su)
        {
            u = su;
        }

        void  setV(double sv)
        {
            v = sv;
        }

        double U()const
        {
           return u;
        }

        double V()const
        {
           return v;
        }

    };

    struct BoundingBox
    {
        Vec3D peaks[8];
        double MinMax[3][2];

        BoundingBox()
        {
            setBounding(0,0,0,0,0,0);
        }

        BoundingBox& operator = (const BoundingBox &other)
        {
            for(int i = 0; i < 8; i++)
            {
                peaks[i] = other.peaks[i];
            }

            for(int i = 0; i < 3; i++)
              for(int j = 0; j < 2; j++)
              {
                  MinMax[i][j] = other.MinMax[i][j];
              }
              return *this;
        }

        void setBounding(double minX,double maxX,double minY,double maxY, double minZ, double maxZ )
        {
            MinMax[0][0] = minX,MinMax[0][1] = maxX,MinMax[1][0] = minY,MinMax[1][1] = maxY,MinMax[2][0] = minZ,MinMax[2][1] = maxZ;
            setPeaks();
        }

        void setPeaks()
        {
            peaks[0] = Vec3D(MinMax[0][0],MinMax[1][0],MinMax[2][0]);
            peaks[1] = Vec3D(MinMax[0][1],MinMax[1][0],MinMax[2][0]);
            peaks[2] = Vec3D(MinMax[0][1],MinMax[1][1],MinMax[2][0]);
            peaks[3] = Vec3D(MinMax[0][0],MinMax[1][1],MinMax[2][0]);

            peaks[4] = Vec3D(MinMax[0][0],MinMax[1][0],MinMax[2][1]);
            peaks[5] = Vec3D(MinMax[0][1],MinMax[1][0],MinMax[2][1]);
            peaks[6] = Vec3D(MinMax[0][1],MinMax[1][1],MinMax[2][1]);
            peaks[7] = Vec3D(MinMax[0][0],MinMax[1][1],MinMax[2][1]);
        }

        void transwithMat(Matrix4X4 mat)
        {
            Vec3D vecs;
            for(int i = 0; i < 2; i++)
            {
                vecs.setX(MinMax[0][i]);vecs.setY(MinMax[1][i]);vecs.setZ(MinMax[2][i]);
                vecs = mat *vecs;
                MinMax[0][i] = vecs.getX();MinMax[1][i] = vecs.getY();MinMax[2][i] = vecs.getZ();
            }
            setPeaks();
        }

        void clear()
        {
            for(unsigned i = 0; i < 8; i++)
               peaks[i] = Vec3D(0,0,0);
        }

        //! get the original bottom-left corner of the bounding rectangle. not after rotted.
        Vec3D getDownBotLeft()
        {
           return peaks[0];
        }

        Vec3D getDownBotRight()
        {
            return peaks[1];
        }

        Vec3D getDownTopLeft()
        {
            return peaks[3];
        }

        Vec3D getDownTopRight()
        {
            return peaks[2];
        }

        Vec3D getUpTopRight()
        {
            return peaks[6];
        }

        double getminX()
        {
            return (MinMax[0][0]);
        }

        double getmaxX()
        {
            return (MinMax[0][1]);
        }

        double getminY()
        {
            return (MinMax[1][0]);
        }

        double getmaxY()
        {
            return (MinMax[1][1]);
        }

        double getminZ()
        {
            return (MinMax[2][0]);
        }

        double getmaxZ()
        {
            return (MinMax[2][1]);
        }


        //!get the peaks.
        /*\return  the longer edge of the bounding rectangle*/
        double getMaxDownEdgeXY(double &minX,double &maxX,double &minY,double &maxY)
        {
            vector<double> xarray,yarray;
            for(unsigned i = 0; i < 8; i++)
            {
                xarray.push_back(peaks[i].getX());
                yarray.push_back(peaks[i].getY());
            }

             sort(xarray.begin(),xarray.end());
             sort(yarray.begin(),yarray.end());

             minX = *(xarray.begin());
             maxX = *(xarray.end()-1);
             minY = *(yarray.begin());
             maxY = *(yarray.end()-1);

             double distY = maxY - minY;
             double distX = maxX - minX;

             return  distY > distX ? distY:distX;
        }

         double getMaxDownEdgeXY()
        {
           double minx,maxx,miny,maxy;
           return getMaxDownEdgeXY(minx,maxx,miny,maxy);
        }

        //!call back function.
        void SrotateZ(Vec3D (*rotz)(Vec3D&, Vec3D&, bool),Vec3D &ThisRot,bool clockwise)
        {
          for(unsigned i = 0; i < 8; i++)
          {
              rotz(peaks[i],ThisRot,clockwise);
          }
        }

    };

    typedef vector<Vec2D> RPpolygon;
    typedef vector<RPline2D>  RPlines2D;
    typedef list<Vec2D> RPNodeList;
    typedef vector<Vec3D> RPVertices;
    typedef vector<Line> Tracks;
    typedef vector<RPDisk> Balls;
    typedef vector<RPVertices> VerticesGrp;
    typedef vector<RPprintItem> RPPrintTrack;

    class LoopOperate
    {
        public:
        LoopOperate(){}

        LoopOperate(unsigned limit):m_limit(limit)
        {}


        int loopAdd(unsigned source, unsigned add)
        {
            if(source > m_limit-1 || add > m_limit -1)
                return -1;

             unsigned target = source + add;

             if(target > m_limit -1)
                target = target - m_limit;

             return target;
        }

        int loopMinus(unsigned source, unsigned sub)
        {
            if(source > m_limit-1 || sub > m_limit -1)
                return -1;

             unsigned target;

             if(source >= sub )
             {
                target = source -sub;
             }
             else
             {
                target = source + m_limit -sub;
             }

             return target;
        }

        int loopDistance(unsigned source, unsigned target)
        {
             if(source > m_limit-1 || target > m_limit -1)
                return -1;

              unsigned result1, result2;
              result1 = loopMinus(target,source);
              result2 = loopMinus(source,target);

              return result1 < result2 ? result1: result2;
        }


        void setLimit(unsigned limit)
        {
            m_limit = limit;
        }

        unsigned getLimit()
        {
            return m_limit;
        }

        private:
        unsigned m_limit;
    };

    enum LaserStauts
    {
        LASER_NULL = 0,
        LASER_ON,
        LASER_OFF,
    };

    class LaserAttribute
    {
      public:
        LaserAttribute():b_full(false),m_status(LASER_NULL){}

        ~ LaserAttribute(){}

        void clear()
        {
            b_full = false;
            m_status = LASER_NULL;
        }

        bool canbePush()
        {
            if(b_full)
                return false;

            return true;
        }

        bool canbePop()
        {
            if(b_full)
                return true;
            return false;

        }

        bool push(LaserStauts status)
        {
            if(b_full)
            {
                return false;
            }
            else
            {
                m_status = status;
                b_full = true;
            }
            return true;
        }

        LaserStauts pop()
        {
            if(b_full)
            {
                b_full = false;
                return m_status;
            }
            return LASER_NULL;
        }

        LaserStauts getLaserStatus()
        {
            return m_status;
        }

        void setLaserStatus(LaserStauts status)
        {
            m_status = status;
        }

      private:
        bool b_full;
        LaserStauts m_status;

    };
}

#endif // DATABASE_H
