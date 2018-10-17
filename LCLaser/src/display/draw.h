/***************************************************************************
                          RPdraw.h  -  description
                             -------------------
    begin                : May 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#ifndef DRAW_RP_H
#define DRAW_RP_H

#include "graph.h"
#include "data.h"
#include "glwin.h"

namespace RenderRP
{
    class RPdraw : public QGLWidget
    {
    Q_OBJECT

    public:

        explicit RPdraw(QGLWidget *parent = 0);
        RPdraw(DataRP::RPData *rpdata, QGLWidget *parent = 0);
        virtual ~RPdraw();
        virtual void draw();
        virtual void listen();
        virtual void listenOK();

        void setWindowSize(int width, int height);
        void setParam(bool bparam);

        void setScale(float scale);
        void setArrows(bool bshow);
        void setAABB(bool bshow);
        void setWireframe(bool bshow);

    protected:

        void setData(DataRP::RPData *RPData);

        float m_scale;
        bool b_arrow;
        bool b_aabb;
        bool b_wireframe;
        bool b_param;
        DataRP::RPData *rp_data;
        int m_width;
        int m_height;

    private:



    };
}

#endif // DRAW_RP_H
