/***************************************************************************
                            draw.cpp  -  description
                             -------------------
    begin                : May 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#include "draw.h"

using namespace RenderRP;

RPdraw::RPdraw(QGLWidget *parent):QGLWidget(parent),b_arrow(false),b_aabb(false),b_wireframe(false),b_param(false)
{
}

RPdraw::RPdraw(DataRP::RPData *rpdata,QGLWidget *parent):QGLWidget(parent),b_arrow(false),b_aabb(false),b_wireframe(false),b_param(false)
{
    setData(rpdata);
}

RPdraw::~RPdraw()
{
}

void RPdraw::setData(DataRP::RPData *rpdata)
{
    if(rpdata != NULL)
        rp_data = rpdata;
    else
        cout<<"rp data is null"<<endl;
}

void RPdraw::draw()
{
}

void RPdraw::listen()
{

}

void RPdraw::listenOK()
{

}

void RPdraw::setScale(float scale)
{
   m_scale = scale;
}

void RPdraw::setArrows(bool bshow)
{
    b_arrow = bshow;
}


void RPdraw::setAABB(bool bshow)
{
    b_aabb = bshow;
}

void RPdraw::setWireframe(bool bshow)
{
    b_wireframe = bshow;
}

void RPdraw::setParam(bool bparam)
{
    b_param = bparam;
}

void RPdraw::setWindowSize(int width, int height)
{
    m_width = width;
    m_height = height;
}
