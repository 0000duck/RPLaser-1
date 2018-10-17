/***************************************************************************
                          matrix4x4.h  -  description
                             -------------------
    begin                : oct. 26 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef MATRIX_4X4_H
#define MATRIX_4X4_H

#include <math.h>
#include <malloc.h>
#include <iostream>
#include <sstream>
#include "Vec4D.h"

using namespace std;

class Matrix4X4
{
    public:
       Matrix4X4();
       Matrix4X4(const Vec4D (&vec)[4]);
       ~Matrix4X4();

       Matrix4X4 inverse();
       Matrix4X4 rotX(double degreen);
       Matrix4X4 rotY(double degreen);
       Matrix4X4 rotZ(double degreen);
       Matrix4X4 translate(double x,double y,double z);
       Matrix4X4 transpose();
       void loadIdentity();

       Matrix4X4& operator=(const Matrix4X4 &other);
       Vec4D operator*(Vec4D vec);
       Vec3D operator*(Vec3D vec);
       Matrix4X4 operator*(Matrix4X4 smat);

       void setValue(const Vec4D (&vec)[4]);
       void setValue(int index, const Vec4D &vec);
       void getRows(Vec4D (&vec)[4]);
       Vec3D getRows(int index);
       void getColumns(Vec4D (&vec)[4]);
       void getColumns(Vec4D (&vec)[4]) const;
       void toPosmat();

       void printMe();
       void printMe(std::stringstream &strstream);

    protected:
       void swap2(double *a,double *b);
       void getTransVal(double &tx, double &ty, double &tz);
       void setTransValue(double x,double y,double z);
       void trim();

    private:
        Vec4D component[4];
};

#endif//MATRIX_4X4_H
