/***************************************************************************
                          glwin.cpp  -  description
                             -------------------
    begin                : Apr. 2 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "glwin.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "QMouseEvent"
#include <QDebug>
#include <math.h>
#include "display.h"

using namespace RenderRP;

GLuint GLwin::m_texID_background = 0;

GLwin::GLwin(QWidget *parent):
    QGLWidget(parent),
    m_projectType(VPERSPECTIVE),
    m_viewScale(100),
    m_lastPos(QPoint(0,0)),
    b_showbackground(true)
{
}

GLwin::~GLwin()
{
}

void GLwin::initializeGL()
{
    glClearColor(0.25,0.25,0.25,0.0);
    glShadeModel(GL_SMOOTH);

    glClearDepth(1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);//GL_LESS
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

	//anti Antialias
	glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};  // ambient light
    GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
    GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);
    float lightPos0[4] = {0, 0, 1, 1}; // positional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    glLightfv(GL_LIGHT1, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightKs);
    float lightPos1[4] = {3, 3, 3, 0}; // positional light
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    // MUST enable each light source after configuration
    glEnable(GL_LIGHTING);

    genRegistMarkTexs();
    loadTextures();

//#ifdef WIN32
//    glEnable(GL_LINE_SMOOTH);
//#endif
//    glEnable( GL_POLYGON_SMOOTH );

//#ifdef WIN32
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines
//#endif
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);  // Antialias the polygons
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLwin::paintGL()
{
    //every thing on screen clear up.
    glClear(GL_DEPTH_BUFFER_BIT |GL_COLOR_BUFFER_BIT);
    renderBackGround();
    rendering();
    renderOverlayer();
}

//void GLwin::resizeGL(int w, int h)
//{
//    glViewport(0,0,(GLsizei)w,(GLsizei)h);
//    setWindowSize(QRect(0,0,w,h));
//    setViewProject();

//    if(m_wizardBtns != NULL)
//        m_wizardBtns->setGeometryPosition(m_windowSize);
//}

//!Zoom
void GLwin::wheelEvent(QWheelEvent *event)
{

    float delta = 1.05f;
    if(event->delta() > 0)
       zoomOut(delta);

    if(event->delta() < 0)
       zoomIn(delta);

    updateGL();
}

void GLwin::mousePressEvent(QMouseEvent * event)
{

   setLastMousePosition(event->pos());
}

void GLwin::mouseMoveEvent(QMouseEvent *event)
{
    float dX = GLfloat(event->x() - getLastMousePosition().x()) / width();
    float dY = GLfloat(event->y() - getLastMousePosition().y()) / height();

    if (event->buttons() & Qt::LeftButton)
    {
       spinView(dX,dY);
    }

    if (event->buttons() & Qt::RightButton)
    {
       transView(dX,dY);
    }

    setLastMousePosition(event->pos());
    updateGL();
}

void GLwin::setWindowSize(QRect wsize)
{
    m_windowSize = wsize;
}

void GLwin::setViewProject()
{

   float aspect = (float)m_windowSize.width()/(float)m_windowSize.height();

  //glPushMatrix();// with it the view will not changned
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

  if(m_projectType == VPERSPECTIVE)
  {
      gluPerspective(m_cam.Persp,aspect,m_viewScale/100, m_viewScale*1000); //key of keep object's shape

  }
  else
  {
      glOrtho(-aspect*m_cam.Zoom, aspect*m_cam.Zoom, -m_cam.Zoom,m_cam.Zoom,-m_viewScale*100,m_viewScale*100); //key of keep object's shape
      //qDebug()<<-aspect*m_cam.Zoom<<" "<<aspect*m_cam.Zoom<<" "<<-m_cam.Zoom<<" "<<m_cam.Zoom<<-m_viewScale*100<<" "<<m_viewScale*100<<endl;
  }

  //glPopMatrix();// with it the view will not changned
}

void GLwin::zoomIn(float delta)
{
   m_cam.Zoom /= delta; //roll down larger

}

void GLwin::zoomOut(float delta)
{
   m_cam.Zoom *= delta; // roll  up  smaller
}

void GLwin::setLastMousePosition(QPoint spos)
{
   m_lastPos = spos;
}

QPoint GLwin::getLastMousePosition()
{
   return m_lastPos;
}

bool GLwin::is2D()
{
  if(m_projectType == VORTHO)
     return true;
  else
     return false;
}

void GLwin::renderBackGround()
{
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, m_windowSize.width(), 0, m_windowSize.height(), -10, 10);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

       drawBackSet();

    glPopMatrix();
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}

void GLwin::renderOverlayer()
{
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, m_windowSize.width(), 0, m_windowSize.height(), -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderOverScene();
    glDisable(GL_BLEND);

    glPopMatrix();
    glPopMatrix();
    updateOverlayGL();
    glEnable(GL_DEPTH_TEST);
}

void GLwin::rendering()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glPushMatrix();
     setViewProject();// 3D scene.

     glMatrixMode(GL_MODELVIEW);

     glPushMatrix();
     glLoadIdentity();
     //trans and rot camera.
     GLTranslateCam();

     renderScene();

     glPopMatrix();
    glPopMatrix();
}


/*****************viewing**********************/
void GLwin::setView()
{

    m_cam = Camera(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,30.0f,200.0f);//perspect,zoom
}

void GLwin::setProjector(ProjectType stype)
{
     m_projectType = stype;
     setView();
}

void GLwin::GLTranslateCam()
{
    //qDebug()<<"zNear"<<-m_cam.Zoom/m_cam.Persp*200<<endl;
    glTranslated(m_cam.XPos,m_cam.YPos,-m_cam.Zoom/m_cam.Persp*200);//perspetive zoom  with inverse compensation
    glRotatef(m_cam.XRot, 1.0f, 0.0f, 0.0f);
    glRotatef(m_cam.YRot, 0.0f, 1.0f, 0.0f);
    glRotatef(m_cam.ZRot, 0.0f, 0.0f, 1.0f);
    glTranslatef(-m_cam.TargetX, -m_cam.TargetY, -m_cam.TargetZ);

}

void GLwin::spinView(GLfloat &dX, GLfloat &dY)
{

 if (m_projectType == VPERSPECTIVE)  //if 3D view
 {
     m_cam.XRot += 300.0f * dY;
     if (m_cam.XRot > 360.0f) m_cam.XRot -= 360.0f;
     if (m_cam.XRot < -360.0f) m_cam.XRot += 360.0f;

//        m_cam.YRot += 300.0f * dX;
//        if (m_cam.YRot > 360.0f) m_cam.YRot -= 360.0f;
//        if (m_cam.YRot < -360.0f) m_cam.YRot += 360.0f;

     m_cam.ZRot += 300.0f * dX;
     if (m_cam.ZRot > 360.0f) m_cam.ZRot -= 360.0f;
     if (m_cam.ZRot < -360.0f) m_cam.ZRot += 360.0f;
 }
 else //if 2D view currently
 {
     m_cam.XPos += 4 * m_cam.Zoom * dX; //change these factors below, as well...
     m_cam.YPos -= 4 * m_cam.Zoom * dY;
 }
}

void GLwin::transView(GLfloat &dX, GLfloat &dY)
{
    m_cam.XPos += 4 * m_cam.Zoom * dX;
    m_cam.YPos -= 4 * m_cam.Zoom * dY;
}


/*****************texture**********************/
void GLwin::loadTextures()
{
    loadBackgroundTex();
}

void GLwin::loadBackgroundTex()
{
    QImage tex,buf;
    if(!buf.load(":/blueBack.png"))
    {
     qWarning("cannot open the image....");
     QImage dummy(128,128,QImage::Format_RGB32);
     dummy.fill(Qt::green);
     buf = dummy;
    }

    tex = convertToGLFormat(buf);
    glGenTextures(1,&m_texID_background);
    glBindTexture(GL_TEXTURE_2D, m_texID_background);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLwin::genRegistMarkTexs()
    {
    for(int i = 0; i < 500; i++)
    {
      glGenTextures(1,&m_texID_registMarks[i]);
    }
    }

    void GLwin::drawBackSet()
    {
    float max_x = m_windowSize.width();
    float max_y = m_windowSize.height();
    float min_x = 0;
    float min_y = 0;
    float hz = 0;

    if(b_showbackground)
    {
    //glColor3f(0.3f,0.3f,0.3f);
    glColor4f(1.0f,1.0f,1.0f,0.0f);
    glBindTexture(GL_TEXTURE_2D,m_texID_background);
    }
    else
    {
    glColor4f(0.1f,0.1f,0.1f,0.0);
    }

    glBegin(GL_TRIANGLE_STRIP);

    glNormal3f(0,0,1);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(min_x,max_y,hz);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(min_x,min_y,hz);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(max_x,max_y,hz);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(max_x,min_y,hz);

    glEnd();
    if(b_showbackground)
    {
    glBindTexture(GL_TEXTURE_2D,0);
    }
}

void GLwin::showBackground(bool bs)
{
    b_showbackground = bs;
}

bool GLwin::isShowBackground()
{
    return b_showbackground;
}


