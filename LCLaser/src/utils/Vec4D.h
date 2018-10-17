/***************************************************************************
                          Vec4D.h  -  description
                             -------------------
    begin                : Jun. 23 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef _VEC4D_H
#define _VEC4D_H

#include "Vec3D.h"

class Vec4D
{
public:
    double component[4];

    Vec4D()
    {
        for(int i = 0; i < 4; i++)
        {
            component[i] = 1;
        }
    }

    Vec4D(const double (&vec)[4])
    {
        setValue(vec);
    }

    Vec4D(double x, double y, double z, double a)
    {
        setValue(x,y,z,a);
    }

    void setValue(double x, double y, double z, double a)
    {
        component[0] = x;
        component[1] = y;
        component[2] = z;
        component[3] = a;
    }

    void setValue(double x, double y, double z)
    {
        component[0] = x;
        component[1] = y;
        component[2] = z;
    }

    void setValue(double a)
    {
         component[3] = a;
    }

    void setValue(const double (&vec)[4])
    {
        for(int i = 0; i < 4; i++)
        {
            component[i] = vec[i];
        }
    }

    void setValue(int index,double value)
    {
        component[index] = value;
    }

    double getValue(int index)
    {
        return component[index];
    }

    double getValue(int index) const
    {
        return component[index];
    }

    Vec4D& operator=(const Vec4D &other)
    {
        for(int i = 0; i < 4; i++)
        {
            component[i] = other.component[i];
        }
        return *this;
    }

    Vec4D operator/(double div)
    {
        Vec4D vec  = *this;
        for(int i = 0; i < 4; i++)
        {
           vec.setValue(component[0]/div,component[1]/div,component[2]/div,component[3]/div);
        }
        return vec;
    }

    Vec4D operator*(double mult)
    {
       Vec4D vec  = *this;
       for(int i = 0; i < 4; i++)
       {
          vec.setValue(component[0]*mult,component[1]*mult,component[2]*mult,component[3]*mult);
       }
       return vec;
    }

    Vec4D operator+(const Vec4D &other)
    {
       Vec4D vec  = *this;
       for(int i = 0; i < 4; i++)
       {
          vec.setValue(component[0]+other.component[0],component[1]+other.component[1],
                       component[2]+other.component[2],component[3]+other.component[3]);
       }
       return vec;
    }

    Vec4D& operator+=(const Vec4D &other)
    {
       for(int i = 0; i < 4; i++)
       {
         component[i] += other.component[i];
       }
       return *this;
    }

    bool operator ==(const Vec4D &other)
    {
       bool result = (component[0] == other.component[0] &&
                      component[1] == other.component[1] &&
                      component[2] == other.component[2] &&
                      component[3] == other.component[3]
                      );
       return result;
    }

    bool operator !=(const Vec4D &other)
    {
       Vec4D vec  = *this;
       return !(vec == other);
    }

    Vec3D toVec3D()
    {
       Vec3D point = Vec3D(component[0],component[1],component[2]);
       return point;
    }
};

#endif // _VEC4D_H
