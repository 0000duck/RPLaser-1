/***************************************************************************
                          RPDisplay.cpp  -  description
                             -------------------
    begin                : Apr. 1st 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QMessageBox>
#include <QDebug>
#include <math.h>
#include <iostream>
#include <vector>
#include <qgl.h>
#include "display.h"
#include "graph.h"
#include "drawSlice.h"
#include "drawAnimation.h"
#include "overlayscene.h"
#include "drawMesh.h"
#include "measure.h"

using namespace std;
using namespace RenderRP;

RPDisplay::RPDisplay(DataRP::RPData *sRPData,QWidget *parent):
    GLwin(parent),
    b_showLaserHead(false),
    b_timershowInf(false),
    m_wizardBtns(NULL)
{
    if(sRPData != NULL)
    {
      rp_data = sRPData;
    }
    else
    {
        QMessageBox::critical(0,QObject::tr("NO Data"), QObject::tr("Data pointer is NULL"));
    }

    createDraw();
    setView();

    if(m_projectType == VPERSPECTIVE)
    {
        float dx = 0.12f;
        float dy = -0.20f;
        spinView(dx,dy);
        zoomIn(2.2f);
    }
    setGrid(20,600,600);
//    if(QGLFormat::hasOpenGLOverlays())
//        qDebug()<<"has OpenGL overLayer"<<endl;
//    else
//        qDebug()<<"-----no OpenGL overLayer---"<<endl;

    m_wizardBtns = new WizardButtons(this);
    m_wizardBtns->hide();
}

RPDisplay:: ~RPDisplay()
{
    delete draw_slices;
    delete draw_animation;
    delete m_wizardBtns;
}

void RPDisplay::resizeGL(int w, int h)
{
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    setWindowSize(QRect(0,0,w,h));
    setViewProject();

    if(m_wizardBtns != NULL)
        m_wizardBtns->setGeometryPosition(m_windowSize);
}

/*****************rendering**********************/

void RPDisplay::renderOverScene()
{
    draw_overlay->setWindowSize(m_windowSize.width(),m_windowSize.height());
    draw_overlay->draw();

    QFont tfont = QFont("courier",14,QFont::Bold);
    if(rp_data->getBar(DataRP::BAR_PRINT))
    {
        glColor3f(0.0f,1.0f,0.0f);
        QString sstr1 = QString::number(rp_data->m_curPrintLevel,10);
        QString sstr = QObject::tr("printing ");
        QString sstr2 = QObject::tr(" layer");
        sstr.append(sstr1);
        sstr.append(sstr2);

        if(rp_data->m_curPrintLevel == (int)rp_data->getMaxLevel())
        {
             sstr = QObject::tr(" finish  ");
             m_wizardBtns->setHideWizardBtn(false,3);

        }
        renderText(0.88*m_windowSize.width(), 0.08*m_windowSize.height(),0,sstr,tfont);//0.91
    }

    if(b_timershowInf)
    {
       tfont = QFont("courier",11,QFont::Bold);
       renderText(0.5*m_windowSize.width(), 0.2*m_windowSize.height(),0,m_timeInfo,tfont);
    }

    showWizardText(m_wizardBtns->getCurrentStep());
}


void RPDisplay::showWizardText(int boldindex)
{
    double xstep = 4;
    double ypos = 25;
    QStringList wizard;
    QFont tfont = QFont("courier",11,QFont::Bold);
    QFont bfont = QFont("courier",14,QFont::Bold);
    wizard<<QObject::tr(" Start -> ")<<QObject::tr("Pos Set -> ");
    wizard<<QObject::tr("Go Home -> ") ;
     //<<QObject::tr("Measure -> ");
    wizard<<QObject::tr("Import -> ")<<QObject::tr("Slice -> ");
    wizard<<QObject::tr("Path -> ")<<QObject::tr("Print ");

    int lsize = wizard.size();
    glColor3f(0.0f,0.0f,0.0f);


    for(int i = 0; i < lsize; i++)
    {
        if(i == boldindex)
        {
            glColor3f(1.0f,1.0f,0.0f);
            renderText(xstep,ypos,wizard[i],bfont);
            glColor3f(0.0f,0.0f,0.0f);
            xstep += wizard[i].size()*20;
        }
        else
        {
            renderText(xstep,ypos,wizard[i],tfont);
            xstep += wizard[i].size()*16;
        }
    }
}

void RPDisplay::renderScene()
{
    Graphs::drawGrid(m_grid);

    draw_animation->setParam(b_showLaserHead);
    draw_animation->draw();

    if(rp_data->meshRenderable())
    {
        if(rp_data->isShowMesh())
             draw_mesh->draw();
    }

    if (rp_data->slicesRenderable() && rp_data->b_canLoad)
    {
        //qDebug()<<"scale   "<<m_cam.Zoom<<endl;
        draw_slices->setScale(m_cam.Zoom);//for draw arrow.
        draw_slices->draw();
    }

        draw_process->setScale(m_cam.Zoom);
        draw_process->draw();

        drawLocationMarks();
}

/*********************setting********************************/
void RPDisplay::setGrid(unsigned sscale, unsigned swidth, unsigned sheight)
{
    m_grid.set_grid(sscale, swidth, sheight);
}

void RPDisplay::setGridScale(unsigned sscale)
{
    m_grid.grid_scale = sscale;
    //emit updateDisplay();
}

void RPDisplay::getGrid(int &tscale,int &twidth,int &theight)
{
    tscale = m_grid.grid_scale;
    twidth = m_grid.grid_width;
    theight = m_grid.grid_height;
}

void RPDisplay::setShowLaser(bool bshow)
{
    b_showLaserHead = bshow;
}

void RPDisplay::clearTracks()
{
    rp_data->clearTracks();
}

void  RPDisplay::createDraw()
{
    draw_slices = new RPDrawSlice(rp_data);
    draw_animation = new RPDrawAnimation(rp_data);
    draw_process = new RPDrawProcess(rp_data);
    draw_overlay =  new RPOverLayerScene(rp_data);
    draw_mesh = new RPDrawMesh(rp_data);
}

void RPDisplay::setArrows(bool bshow)
{
    draw_slices->setArrows(bshow);
}

void RPDisplay::setAABB(bool bshow)
{
    draw_slices->setAABB(bshow);
}

void RPDisplay::setshowWireframe(bool bshow)
{
    draw_mesh->setWireframe(bshow);
}

void RPDisplay::spinAndzoom(float spinx,float spiny, float zin)
{
    spinView(spinx,spiny);
    zoomIn(zin);
}

void RPDisplay::listern()
{
   draw_process->listen();
}

void RPDisplay::listernOK()
{
   draw_process->listenOK();
}

void RPDisplay::gotoComdPos(unsigned spos)
{
    draw_process->gotoCommd(spos);
}

DataRP::RPData* RPDisplay::getdata()
{
return rp_data;
}

void RPDisplay::on_showInfo(QString info)
{
    timeShowInf(info,10000);
    updateGL();
}

void  RPDisplay::on_showSteer(bool bshow)
{
    m_wizardBtns->setHideMoveSteer(!bshow);
}

void RPDisplay::timeShowInf(QString text, double dur)
{
    if(m_timer.isActive())
       m_timer.stop();

    if(text == "")
    {
        b_timershowInf = false;
        return;
    }

    m_timer.start(dur,this);
    m_timeInfo = text;
    b_timershowInf = true;
}

void RPDisplay::timerEvent(QTimerEvent *event)
{
     Q_ASSERT(event->timerId() > 0);
         m_timer.stop();
     b_timershowInf = false;
     m_timeInfo = "";
     emit updateGL();
}

void RPDisplay::drawLocationMarks()
{
    SampleMap::iterator sIt;
    Vec3D pos;
    Measure *measure = getMeasure();
    SampleMap *printmap = measure->getPrintSamples();

    if(printmap->size() < 1)
        return;

    for(sIt = printmap->begin();sIt !=  printmap->end(); sIt++)
    {
        pos = sIt->second.toVec3D();
        drawMarksTex(sIt->first,pos);
    }
}

void RPDisplay::genMarkTex(int index)
{
    QString str = QString::number(index);
    QString texPath;

    int mod = index%3;
    switch(mod)
    {
    case 1:
         texPath = ":/mark-g.png";
    break;
    case 2:
         texPath = ":/mark-b.png";
    break;
    default:
         texPath = ":/mark-y.png";
    break;
    }

    QImage image;

    if(!image.load(texPath))
    {
        QImage dummy(256,256,QImage::Format_ARGB32);
        dummy.fill(Qt::green);
        image = dummy;
    }

    QPainter painter(&image); //create a painter for this image.
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);//CompositionMode_DestinationOver

    //pen and font.
    QPen pen = painter.pen();
    pen.setColor(Qt::red);
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(172);

    painter.setPen(pen);
    painter.setFont(font);

    painter.drawText(image.rect(),Qt::AlignCenter,str);

    QImage tex= convertToGLFormat(image);
    glBindTexture(GL_TEXTURE_2D, m_texID_registMarks[index]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    image.fill(0);
    tex.fill(0);
}

void RPDisplay::drawMarksTex(int index,Vec3D pos)
{
    glPushAttrib(GL_CURRENT_BIT);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    //glDepthMask(false);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);

    glBindTexture(GL_TEXTURE_2D,m_texID_registMarks[index]);

    glTranslatef(pos.getX(),pos.getY(),pos.getZ());
    glBegin(GL_QUADS);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 5.0f, 5.0f, 0.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.0f, 5.0f, 0.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f,-5.0f, 0.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 5.0f,-5.0f, 0.0f);
    glEnd();
    glTranslatef(-pos.getX(),-pos.getY(),-pos.getZ());

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    //glDepthMask(true);
    glBindTexture(GL_TEXTURE_2D,0);

    glPopAttrib();
}

void RPDisplay::setWizardButtons(MainWindow *mwin)
{
    m_wizardBtns->setMainWin(mwin);
}

Measure* RPDisplay::getMeasure()
{
    return rp_data->getMeasure();
}
