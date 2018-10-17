/***************************************************************************
                          displaybase.h  -  description
                             -------------------
    begin                : Apr. 29 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DISPLAY_BASE_H
#define DISPLAY_BASE_H

#include <qopengl.h>
#include <GL/glu.h>

namespace RenderRP
{
    enum ViewType
    {
        VTOP,
        VBOTTOM,
        VLEFT,
        VRIGHT,
        VBACK,
    };

    enum ProjectType
    {
        VPERSPECTIVE,
        VORTHO
    };

    enum ZoomType
    {
        ZBOUNDS,
        ZEXTENTS,
        ZSELECTED
    };

    struct Camera
    {
    Camera(){}

    Camera(GLfloat sXPos, GLfloat sYPos, GLfloat sXRot, GLfloat sYRot,GLfloat sZRot,
           GLfloat sTargetX, GLfloat sTargetY, GLfloat sTargetZ,
           GLfloat sPersp,GLfloat sZoom)
    {
        XPos = sXPos, YPos = sYPos, XRot = sXRot,  YRot = sYRot,  ZRot = sZRot,
        TargetX = sTargetX, TargetY = sTargetY, TargetZ = sTargetZ;
        Persp = sPersp; Zoom = sZoom;
    }


    //why without Zpos
    GLfloat XPos, YPos, XRot, YRot,ZRot,
            TargetX, TargetY,TargetZ,
            Persp,Zoom;
    };

    struct Grid
    {
        unsigned grid_scale;
        unsigned grid_width;
        unsigned grid_height;

        void set_grid(unsigned &sscale,unsigned &swidth,unsigned &sheight)
        {
              grid_scale = sscale;
              grid_width = swidth;
              grid_height = sheight;
        }
    };
}

#endif    //DISPLAY_BASE_H
