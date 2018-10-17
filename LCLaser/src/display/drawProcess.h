/***************************************************************************
                          drawProcess.h  -  description
                             -------------------
    begin                : Jul. 3 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DRAW_PROCESS_H
#define DRAW_PROCESS_H

#include <QObject>
#include <QBasicTimer>
#include "draw.h"
#include "dataDynamic.h"

namespace RenderRP
{
    class RPDrawProcess:public RPdraw
    {
    Q_OBJECT
    public:
        explicit RPDrawProcess();
        RPDrawProcess(DataRP::RPData *rpData);
        ~RPDrawProcess();
        virtual void draw();
        virtual void listen();
        virtual void listenOK();
        void gotoCommd(unsigned spos);

    protected:
        bool canbeDisplay();
        void clearStatus();

        void showBalls(DataRP::AnimCommand &commd);
        void deleteBall(DataRP::AnimCommand &commd);
        void showCursor(DataRP::AnimCommand &commd);
        void moveBall(DataRP::AnimCommand &commd);
        void contactLines();
        void showBiLine(DataRP::AnimCommand &commd);
        void finishLoop(DataRP::AnimCommand &commd);
        void finishAll();

        void timerEvent(QTimerEvent *event);
        void parseCommd();

        void drawBalls();
        void drawCursor();
        void drawLines();

        void drawbiLine();
        void drawContactLines();
        void drawLinks();
        void drawContour();
        void reset();



    private:
        QBasicTimer m_timer;
        unsigned m_precursor;
        unsigned m_cursor;

        unsigned m_start;

        unsigned m_list_Cursor;
        unsigned m_contactLine_cursor;

        DataRP::BiLine m_biLine;
        bool b_cursor;
        bool b_showBalls;
        bool b_showLinks;
        bool b_contactLine;
        double m_radius;
        DataRP::SimuBallRoll *m_simuBall;

    };
}

#endif // DRAW_PROCESS_H
