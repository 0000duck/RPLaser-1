/***************************************************************************
                          display.h  -  description
                             -------------------
    begin                : Apr. 1st 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#ifndef DISPLYRP_H
#define DISPLYRP_H

#include "data.h"
#include "gtmock.h"
#include "draw.h"
#include "drawProcess.h"
#include "glwin.h"
#include "wizardbuttons.h"

class MainWindow;
class Measure;

namespace RenderRP
{
    class RPDisplay:public GLwin
    {
        Q_OBJECT
    public:

            RPDisplay(DataRP::RPData *sRPData,QWidget *parent = 0);
            ~RPDisplay();

            DataRP::RPData *getdata();

            Measure *getMeasure();

            void setShowLaser(bool bshow);
            void setArrows(bool bshow);
            void setAABB(bool bshow);
            void setshowWireframe(bool bshow);

            void clearTracks();

            void setGrid(unsigned sscale,unsigned swidth,unsigned sheight);
            void setGridScale(unsigned sscale);
            void getGrid(int &tscale,int &twidth,int &theight);

            void spinAndzoom(float spinx,float spiny, float zin);
            void timeShowInf(QString text, double dur);

            void drawLocationMarks();
            void genMarkTex(int index);
            void drawMarksTex(int index,Vec3D pos);
            void setWizardButtons(MainWindow *mwin);

      public slots:
            void listern();
            void listernOK();
            void gotoComdPos(unsigned spos);
            void on_showInfo(QString info);
            void on_showSteer(bool bshow);

       protected:
            void resizeGL(int w,int h);
            void timerEvent(QTimerEvent *event);
            virtual void renderScene();
            virtual void renderOverScene();

            void createDraw(); //prepare for the draw list.
            void showWizardText(int boldindex);



        private:
            Grid m_grid;

            bool b_showLaserHead;
            bool  b_timershowInf;
            DataRP::RPData *rp_data;

            RPdraw* draw_slices;
            RPdraw* draw_animation;
            RPdraw* draw_overlay;
            RPdraw* draw_mesh;
            RPDrawProcess* draw_process;
            QBasicTimer m_timer;
            QString m_timeInfo;

            WizardButtons *m_wizardBtns;
    };
}
#endif // DISPLYRP_H
