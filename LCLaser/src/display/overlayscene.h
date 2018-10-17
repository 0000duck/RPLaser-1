/***************************************************************************
                          overlayscene.h  -  description
                             -------------------
    begin                :Sep. 28 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef OVER_LAYER_SCENE_H
#define OVER_LAYER_SCENE_H

#include "draw.h"

namespace RenderRP
{
    struct ProgressBar
    {
        float m_wdelta;
        float m_hdelta;
        float m_minx, m_miny,m_maxx,m_maxy;
        float m_percent;

        ProgressBar():m_percent(0.0f)
        {}

        ProgressBar(float rminx,float rmaxx,float rminy,float rmaxy):m_percent(0.0f)
        {
         setBarSize(rminx,rmaxx,rminy,rmaxy);
        }

         ~ProgressBar(){}

        void setWindowSize(float wwidth, float wheight)
        {
            m_wdelta = (float)wwidth/100.0f;
            m_hdelta = (float)wheight/100.0f;
        }

        void setPercent(float percent)
        {
            m_percent = percent;
        }

        void setBarSize(float rminx,float rmaxx,float rminy,float rmaxy)
        {
            m_minx = rminx;
            m_maxx = rmaxx;
            m_miny = rminy;
            m_maxy = rmaxy;
        }

        void drawBar()
        {
            drawEmptyBar();

            if(m_percent > 0)
                drawPercent();
        }

        void drawEmptyBar()
        {
            GLfloat minX = m_minx * m_wdelta;
            GLfloat maxX = m_maxx * m_wdelta;
            GLfloat minY = m_miny * m_hdelta;
            GLfloat maxY = m_maxy * m_hdelta;

            DataRP::RPpolygon vecs;
            vecs.push_back(Vec2D(maxX,maxY));
            vecs.push_back(Vec2D(minX,maxY));
            vecs.push_back(Vec2D(minX,minY));
            vecs.push_back(Vec2D(maxX,minY));
            Graphs::draw2DFrameBar(vecs);
        }

        void drawPercent()
        {
            GLfloat minX = m_minx * m_wdelta;
            GLfloat maxX = m_maxx * m_wdelta;
            GLfloat minY = m_miny * m_hdelta;
            GLfloat maxY = m_maxy * m_hdelta;

            float percentHeight = minY + (maxY - minY)*m_percent;
            DataRP::RPpolygon vecs;
            vecs.push_back(Vec2D(maxX,percentHeight));
            vecs.push_back(Vec2D(minX,percentHeight));
            vecs.push_back(Vec2D(minX,minY));
            vecs.push_back(Vec2D(maxX,minY));
            Graphs::draw2DBar(vecs);
        }
    };


    class RPOverLayerScene:public RPdraw
    {
    public:
        RPOverLayerScene(DataRP::RPData *rpdata);
        virtual ~RPOverLayerScene();
        virtual void draw();
    protected:



    private:
        ProgressBar *rightBar;
        ProgressBar *leftBar;
    };
}

#endif // OVER_LAYER_SCENE_H
