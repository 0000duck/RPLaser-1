/***************************************************************************
                          drawMesh.h  -  description
                             -------------------
    begin                : Jun. 3 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DRAW_MESH_H
#define DRAW_MESH_H

#include "draw.h"

namespace RenderRP
{
    class RPDrawMesh:public RPdraw
    {
    public:
        RPDrawMesh();
        RPDrawMesh(DataRP::RPData *rpdata);
        virtual ~RPDrawMesh();
        virtual void draw();

    protected:
        void drawSolid();
        void drawWireFrame();
        void drawNormals();
    };
}

#endif // DRAW_MESH_H
