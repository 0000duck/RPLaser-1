/***************************************************************************
                          drawSlice.h  -  description
                             -------------------
    begin                : Jun. 3 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DRAW_SLICE_H
#define DRAW_SLICE_H

#include "draw.h"

namespace RenderRP
{
    class RPDrawSlice:public RPdraw
    {
    public:
        RPDrawSlice();
        RPDrawSlice(DataRP::RPData *rpdata);
        virtual ~RPDrawSlice();
        virtual void draw();

    protected:
        void drawCutPlane();
        void drawBoundaries(DataRP::RPslice slice);
        void drawPaths(int currentlayer);
        void drawPathOutline();
        void drawPathParallel();
        void drawAABB();


    private:

    };
}

#endif // DRAW_SLICE_H
