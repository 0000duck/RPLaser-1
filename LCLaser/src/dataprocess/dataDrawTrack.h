/***************************************************************************
                          dataDrwTrack.h  -  description
                             -------------------
    begin                : Aug. 26 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DATA_DRAW_TRACK_H
#define DATA_DRAW_TRACK_H

#include "database.h"

namespace DataRP
{

    enum TRACK_TYPE
    {
      TRACK_TERMINAL = 0,
      TRACK_LINE,
      TRACK_ELBOW,
      TRACK_ARC,
    };

    struct LastTrackItem
    {
       Line lastLine;
       Line lastScratch;
       double lastRadius;

       LastTrackItem(){}

       LastTrackItem(double sradius):
       lastRadius(sradius)
       {}

       LastTrackItem(Line line, Line scratch,double sradius):
       lastLine(line),lastScratch(scratch),lastRadius(sradius)
       {}

       ~LastTrackItem(){}

       void initiate(double sradius)
       {
           Vec3D pos=Vec3D(0.0,0.0,-1.0);
           lastLine = Line(pos,pos);
           lastRadius= sradius;
       }

       void setValue(Line &line, Line & scratch)
       {
           lastLine = line;
           lastScratch = scratch;
       }

       void setLine(Line &line)
       {
            lastLine = line;
       }

       void setScratch(Line &scratch)
       {
            lastScratch = scratch;
       }

       void setRadius(double sradius)
       {
           lastRadius= sradius;
       }

       double getRadius()
       {
           return lastRadius;
       }

       Vec3D getLastTail()
       {
           return lastLine.getTail();
       }

       Vec3D getLastHead()
       {
           return lastLine.getHead();
       }

       void getScratch(Vec3D &leftend, Vec3D &rightend)
       {
           leftend = lastScratch.getTail();
           rightend = lastScratch.getHead();
       }

       Line getLine()
       {
           return lastLine;
       }

       void getLineVertials(Vec3D &left, Vec3D &right)
       {
           lastLine.getVertials(left,right);
       }

       void getLineEnd(Vec3D &leftend, Vec3D &rightend, double radius)
       {
           Vec3D normal = lastLine.getVector();
           Vec3D right = normal.VerticalXYRight();
           Vec3D left = normal.VerticalXYLeft();
           Vec3D tend = lastLine.getTail();

           leftend = left  * radius + tend;
           rightend= right * radius + tend;
       }

       //return left vector.
       Vec2D getStartScratch(Vec3D &leftHead,Vec3D &rightHead)
       {
           Vec3D normal = lastLine.getVector();
           Vec3D thead = lastLine.getHead();

           Vec3D left = normal.VerticalXYLeft();
           Vec3D right = normal.VerticalXYRight();

           leftHead = left  * lastRadius + thead;
           rightHead= right * lastRadius + thead;

           Vec2D vec =  Vec2D(left.getX(),left.getY());
           return vec;
       }

       Vec2D getEndScratch(Vec3D &leftEnd,Vec3D &rightEnd)
       {
           Vec3D normal = lastLine.getVector();
           Vec3D tail = lastLine.getTail();

           Vec3D left = normal.VerticalXYLeft();
           Vec3D right = normal.VerticalXYRight();

           leftEnd = left  * lastRadius + tail;
           rightEnd= right * lastRadius + tail;

           Vec2D vec =  Vec2D(left.getX(),left.getY());
           return vec;
       }
    };

    class DataDrawTrack
    {
    public:
        DataDrawTrack();
        DataDrawTrack(int stype);
        DataDrawTrack(int stype,double sradius);
        virtual ~DataDrawTrack();
        int getType();
        double getTrackWidth();
        virtual void getParam(Vec3D &va,Vec3D &vb,Vec3D &vc,Vec3D &vd,double &radius, double &radian,short &dir) = 0;

    private:
        int m_type;
        double m_trackWidth;

    };


    class TrackTerminal :public DataDrawTrack
    {
    public:
        TrackTerminal():DataDrawTrack(TRACK_TERMINAL){}

        ~TrackTerminal(){}

        TrackTerminal(Vec3D scenter,Vec3D svec,double trackWidth);

        virtual void getParam(Vec3D &va,Vec3D &vb,Vec3D &vc,Vec3D &vd,double &radius, double &radian,short &dir);

    private:
        Vec3D m_center;
        Vec3D m_vertVec;
    };

    class TrackLine :public DataDrawTrack
    {
    public:
        TrackLine():DataDrawTrack(TRACK_LINE){}

        ~TrackLine(){}

        TrackLine(Vec3D leftstart,Vec3D rightstart,Vec3D leftend,Vec3D rightend);

        virtual void getParam(Vec3D &va,Vec3D &vb,Vec3D &vc,Vec3D &vd,double &radius,double &radian,short &dir);

    private:
        Vec3D m_leftStart;
        Vec3D m_rightStart;
        Vec3D m_leftEnd;
        Vec3D m_righEnd;
    };


    class TrackElbow :public DataDrawTrack
    {
    public:
        TrackElbow():DataDrawTrack(TRACK_ELBOW){}

        ~TrackElbow(){}

        TrackElbow(Vec3D start,Vec3D send,Vec3D interPoint,Vec3D center, double trackWidth,short direction);

        virtual void getParam(Vec3D &va,Vec3D &vb,Vec3D &vc,Vec3D &vd,double &radius, double &radian, short &dir);

    private:
        Vec3D m_start;
        Vec3D m_end;
        Vec3D m_interpos;
        Vec3D m_center;
        short m_dir;
    };

    class TrackArc :public DataDrawTrack
    {
    public:
        TrackArc():DataDrawTrack(TRACK_ARC){}

        ~TrackArc(){}

        TrackArc(Vec3D start,Vec3D send,Vec3D center, double sradius, double radian, double trackWidth);

        virtual void getParam(Vec3D &va,Vec3D &vb,Vec3D &vc,Vec3D &vd,double &radius,double &radian,  short &dir);

    private:
        Vec3D m_start;
        Vec3D m_end;
        Vec3D m_center;
        double m_radius;
        double m_radian;
    };

    typedef vector<DataDrawTrack*> DrawTracks;
    typedef list<DrawTracks>LayerTracks;
}

#endif // DATA_DRAW_TRACK_H
