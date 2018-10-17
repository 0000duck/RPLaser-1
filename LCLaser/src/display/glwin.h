/***************************************************************************
                          glwin.h  -  description
                             -------------------
    begin                : Apr. 2 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef MYGLW_H
#define MYGLW_H


#include <QWheelEvent>
#include <QRect>
#include <QPoint>
#include <QGLWidget>
#include "displaybase.h"


namespace RenderRP
{
    class GLwin : public QGLWidget
    {
         Q_OBJECT
    public:
        explicit GLwin(QWidget *parent = 0);

        ~GLwin();

        void showBackground(bool bs);
        bool isShowBackground();
        bool is2D();
        void setProjector(ProjectType stype);

    signals:

    public slots:

    protected:
        void initializeGL();
        //void resizeGL(int w,int h);
        void paintGL();

        void setViewProject();
        void mousePressEvent(QMouseEvent * event);
        void wheelEvent(QWheelEvent *event);
        void mouseMoveEvent(QMouseEvent *event);

        void setWindowSize(QRect wsize);

        void loadTextures();
        void loadBackgroundTex();
        void genRegistMarkTexs();
        void drawBackSet();

        void zoomIn(float delta);
        void zoomOut(float delta);

        void setLastMousePosition(QPoint spos);
        QPoint getLastMousePosition();

        void spinView(GLfloat &dX, GLfloat &dY);
        void transView(GLfloat &dX, GLfloat &dY);

        void setView();
        void GLTranslateCam(void);

        void renderBackGround();
        void renderOverlayer();
        void rendering();

        virtual void renderScene() = 0;
        virtual void renderOverScene() = 0;


        ProjectType m_projectType;
        float m_viewScale;
        Camera m_cam;
        QPoint m_lastPos;
        bool b_showbackground;
        QRect m_windowSize;
        int m_overWidth;
        int m_overHeight;
    protected:
    GLuint m_texID_registMarks[500];

    public:
        static GLuint m_texID_background;

    };
}
#endif // MYGLW_H
