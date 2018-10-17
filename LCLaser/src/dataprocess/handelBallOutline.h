/***************************************************************************
                          handelBallOutline.h  -  description
                             -------------------
    begin                : Jul. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef HANDEL_BALL_OUTLINE_H
#define HANDEL_BALL_OUTLINE_H

#include "dataDynamic.h"
#include "handelscanpath.h"

namespace DataRP
{
   enum ERListType
   {
     ERLIST_SELFCROSS = 0,
     ERLIST_CURSOR,
     ERLIST_PRECURSOR,
   };

   enum CHECKType
   {
       CHECK_NORMAL = 0,
       CHECK_LOOPOVER,
       CHECK_SELFCROSS,
       CHECK_OTHERCROSS,
       CHECK_TOOSMALL,
       CHECK_NOCLOSE,
   };

   struct BallItem
   {
        unsigned contactIndex;
        int endtype;
        Vec2D interSecPos;

        BallItem(){}

        BallItem(unsigned sindex, int sendtype, Vec2D spos):
        contactIndex(sindex),endtype(sendtype),interSecPos(spos)
        {
        }

        unsigned getIndex()
        {
            return contactIndex;
        }

        int getEndType()
        {
            return endtype;
        }

        Vec2D getInterSecPos()
        {
            return interSecPos;
        }

        BallItem& operator=(const BallItem& other)
        {
            contactIndex = other.contactIndex;
            interSecPos = other.interSecPos;
            endtype = other.endtype;
            return *this;
        }

   };

   typedef vector<BallItem> ERList;

    struct ERItem
    {
        ERList erlist;
        ERListType listType;

        ERItem():listType(ERLIST_SELFCROSS){}

        void clear()
        {
            erlist.clear();
            listType = ERLIST_SELFCROSS;
        }

        void setType(ERListType stype)
        {
            listType = stype;
        }

        ERListType getType()
        {
            return listType;
        }

        unsigned getLength()
        {
            return erlist.size();
        }

    };

    typedef vector<ERItem> ERListGroup;


    class HandelBallOutline :public HandelScanPath
    {
    public:
        HandelBallOutline(RPData *rpdata);
        ~HandelBallOutline();

    protected:

       bool genAllBalls();
       int rollBall(RPcontour &contour);
       int ballonLine();
       int checkSelfcross();
       double disForward2Pos(unsigned sindex);
       double distBackward2Pos(unsigned sindex);

       bool getSelfCutPoint(ERItem &item);
       int isIntersectLine(RPline2D &line1, RPline2D &line2, Vec2D &spoint);

       bool isIntsectOther(RP2DDisk &ball);
       bool isInterOtherOutline(RP2DDisk &ball,RPlines2D &linelist,unsigned hitIndex);

       void unlashSelfLoop(int result,unsigned curIndex,Vec2D &intsecPos);
       void unlashOtherLoop(RPlines2D &linelist,int result,unsigned hitIndex,Vec2D &intsecPos);

       void getAniCommdsSize();

       bool ballInterLine(RP2DDisk &ball,unsigned currIndex,BallItem &Item);

    private:
       double m_curZ;
       RPlines2D m_linelist;
       double m_circum;

       unsigned m_scursor;  //static cursor for count loop.
       unsigned m_cursor;
       unsigned m_precursor;
       unsigned m_limit;
       unsigned m_loopcount;

       Vec2D m_v2base;
       RollBalls m_balls;

       unsigned m_neigbourRange;
       unsigned m_disloop;
       bool b_exitloop;
       LoopOperate m_lop;

       RPboundlist m_boundlist;
       RPboundaries m_boundrefer;

       RPcontour m_contour1;
       RPcontour m_contour2;

       unsigned m_cnumb;
       bool b_simuBall;
       virtual void genOutline(RPboundaries &inputboundaries,RPboundaries &outputboundaries);

    };
}

#endif // HANDEL_BALL_OUTLINE_H
