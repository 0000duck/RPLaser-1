/***************************************************************************
                          drawAnimation.h  -  description
                             -------------------
    begin                : Jun. 11 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DRAW_ANIMATION_H
#define DRAW_ANIMATION_H

#include "draw.h"

namespace RenderRP
{
    class RPDrawAnimation:public RPdraw
    {
    public:
        RPDrawAnimation();
        RPDrawAnimation(DataRP::RPData *rpData);
        virtual ~RPDrawAnimation();
        virtual void draw();

    protected:
        void drawLaserHead();
        void drawLayerTracks();
        void drawTracks(DataRP::DrawTracks &drawTracks);
        void drawAnimTrack();
        void drawAnimArc();



    private:


    };
}

#endif // DRAW_ANIMATION_H
