/***************************************************************************
                          Vec3D.cpp  -  description
                             -------------------
    begin                : Jun. 23 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "Vec2D.h"
#include "Vec3D.h"
#include "Vec4D.h"

Vec4D Vec3D::toVec4D()
{
    return Vec4D(x,y,z,1);
}

Vec2D Vec3D::toVec2D()
{
    return Vec2D(x,y);
}
