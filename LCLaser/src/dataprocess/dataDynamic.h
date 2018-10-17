/***************************************************************************
                          dataDynamic.h  -  description
                             -------------------
    begin                : Apr. 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
    description           for animation and simulation.
 ***************************************************************************/
#ifndef DATADYNAMIC_H
#define DATADYNAMIC_H

#include "dataSlice.h"

namespace DataRP
{
/**************dynamic  Data******************/
    enum ANIM_PROCESS
    {
        ANP_WAIT = 0,
        ANP_SHOW_BALLS,
        ANP_MOVE_BALL,
        ANP_DELETE_BALL,
        ANP_UPDATE_CURSOR,
        ANP_HELP_LINE,
        ANP_CONTACT_LINES,
        ANP_FARCONTACT_LINES,
        ANP_LINK_LINES,
        ANP_CONTOUR_LOOP,
        ANP_FINISH_LOOP,
        ANP_FINISH_ALL,
    };

    enum  DRAWED_BALL
    {
         DR_BALL_NORMAL = 0,
         DR_BALL_DELETED,
         DR_BALL_MOVEING,
         DR_BALL_ALTMOVEING,
         DR_BALL_CURRENT,
    };

    //animation.
    enum TraverseType
    {
        TRAVEL_EMPTY = 0,
        TRAVEL_PRINT,
        TRAVEL_LASERON,
        TRAVEL_LASEROFF,
        TRAVEL_LAYEROVER,
        TRAVEL_FINISH,
    };

    struct PathPoint
    {
       Vec2D vertex;
       Vec3D clockDirection;
       static Vec2D farpoint;

       PathPoint():vertex(0,0),clockDirection(0,0,0)
       {

       }

       PathPoint(double sx, double sy, Vec3D &svc):vertex(sx,sy)
        {
           clockDirection = svc;
        }

       double Dist2(const Vec2D& v) const
        {
            return((v.x -vertex.x)*(v.x -vertex.x) +  (v.y -vertex.y)*(v.y -vertex.y));
        }

      //! compares two PathPoint, who is farther from a specified point.
       static bool compare(const PathPoint &first,const PathPoint &second )
       {
           return first.Dist2(PathPoint::farpoint) < second.Dist2(PathPoint::farpoint);
       }

       static void setFarpoint(Vec3D &point)
       {
           farpoint.setX(point.getX());
           farpoint.setY(point.getY());
       }


    };

    struct RollBall
    {
        Vec2D center;
        double radius;
        Vec2D intersec1;
        Vec2D intersec2;
        int bumptype;
        bool berase;
        int status;

        RollBall():
        center(Vec2D()),radius(1), intersec1(Vec2D()),  intersec2(Vec2D()), berase(false),status(DR_BALL_NORMAL)
        {
        }

        RollBall(const Vec2D &scenter,double sradius, const Vec2D &sintersec1,const Vec2D &sintersec2, int sbump):berase(false),status(DR_BALL_NORMAL)
        {
            center = scenter;
            radius = sradius;
            intersec1 = sintersec1;
            intersec2 = sintersec2;
            bumptype = sbump;
        }

        RollBall& operator = (const RollBall &other)
        {
            center    = other.center;
            radius    = other.radius;
            intersec1 = other.intersec1;
            intersec2 = other.intersec2;
            bumptype  = other.bumptype;
            berase = other.berase;
            status = other.status;
            return *this;
        }

        void setErase(bool sberase)
        {
           berase = sberase;
        }

        bool isErase()
        {
            return berase;
        }

        void setStatus(int st)
        {
            status = st;
        }

        int getStatus()
        {
            return status;
        }

        Vec2D getCenter()
        {
            return center;
        }
    };

        typedef vector<PathPoint> PathPoints;
        typedef vector<PathPoints> PathContainer;
        typedef vector <RollBall> RollBalls;

    struct MovePosition
    {
       double target[3]; //!< target position.
       double curr[3];   //!< current position.
       double dist[3];   //!< moving distance at current postion.
       double step[3];   //!< moving distance of every frame.
       double start[3];  //!< start postion.
       double lenth;     //!< whole lenth of path.

       MovePosition()
       {
           clear();
       }

       void clear()
       {
           for(int i = 0; i < 3; i++)
           {
             target[i] = 0.0;
               curr[i] = 0.0;
               dist[i] = 0.0;
               step[i] = 0.0;
              start[i] = 0.0;
                 lenth = 0.0;
           }
       }

       ~MovePosition(){}
    };

    struct AnimateTrack: public Line
    {
        bool b_active;

        AnimateTrack():Line(),b_active(false)
        {}

        void refreshTrack(Vec3D send)
        {
            ltail = send;
        }

        void setActive(Vec3D start)
        {
            b_active = true;
            lhead = start;
            ltail = start;
        }

        void disActive()
        {
            b_active = false;
        }


        bool isActive()
        {
            return b_active;
        }

    };

     struct AnimateArc
    {
        bool b_active;
        Vec3D m_start;
        Vec3D m_center;
        double m_radius;
        double m_radian;
        double m_stepRadian;

        AnimateArc():b_active(false)
        {}

        void refreshArc(double sradian)
        {
            m_stepRadian = sradian;
        }

        void setActive(Vec3D &start,Vec3D &center, double radius,double radian)
        {
            m_start = start;
            m_center = center;
            m_radius = radius;
            m_radian = radian;
            b_active = true;
            m_stepRadian = 0.0;
        }

        void disActive()
        {
            b_active = false;
        }

        bool isActive()
        {
            return b_active;
        }

        void getAnimValue(Vec3D &start,Vec3D &center, double &radian, double &radius)
        {
            start = m_start;
            center = m_center;
            radian = m_stepRadian;
            radius = m_radius;
        }

        void getValue(Vec3D &start,Vec3D &center, double &radian, double &radius)
        {
            start = m_start;
            center = m_center;
            radian = m_radian;
            radius = m_radius;
        }

        void getcurrPos(Vec3D &pos)
        {
            double radius;
            Vec3D vrot;
            Vec3D vstart = m_start - m_center;
            vstart.Normalize();

            if(m_radius == -1)
                radius = -m_radius;
            else
                radius = m_radius;

            vrot = vstart;
            vrot.RotZ(m_stepRadian);
            pos = vrot * radius + m_center;
        }
    };


    struct BiLine
    {
        Line line1;
        Line line2;
        bool bshow;

        BiLine():line1(Line()),line2(Line()),bshow(false)
        {}

        BiLine(const Line &l1, const Line &l2)
        {
            line1 = l1;
            line2 = l2;
        }

        BiLine& operator =(const BiLine &other)
        {
            line1 = other.line1;
            line2 = other.line2;
            return *this;
        }

        bool isShow()
        {
            return bshow;
        }

        void setShow(bool sshow)
        {
            bshow = sshow;
        }

    };

    struct CommdData
    {
        BiLine biLine;
        Vec3D center;
        Tracks contactLines;


        CommdData()
        {}

        void setBiLine(const BiLine &sbiline)
        {
            biLine = sbiline;
        }

        void setCenter(const Vec3D &scenter)
        {
            center = scenter;
        }

        void clear()
        {
           biLine = BiLine();
           center = Vec3D();
           contactLines.clear();
        }

        CommdData& operator=(const CommdData &other)
        {
            biLine = other.biLine;
            center = other.center;
            std::copy(other.contactLines.begin(),other.contactLines.end(),std::back_inserter(contactLines));
            return *this;
        }

    };
    //!
    //! \brief process animation command;
    //!
    struct AnimCommand
    {
        ANIM_PROCESS command;
        unsigned index;
        unsigned value;
        CommdData DataAppend;

        AnimCommand()
        {
            clear();
        }

        AnimCommand(ANIM_PROCESS scommd, unsigned sindex,unsigned svalue = 0)
        {
            command = scommd;
            index = sindex;
            value = svalue;
        }

        void clear()
        {
            command = ANP_WAIT;
            index = 0;
            value = 0;
            DataAppend.clear();
        }

        ANIM_PROCESS getCommdName() const
        {
            return command;
        }

        unsigned getIndex() const
        {
            return index;
        }

        unsigned getValue() const
        {
            return value;
        }

        void setValue(unsigned svalue)
        {
           value = svalue;
        }


        AnimCommand& operator=(const AnimCommand &other)
        {
            command = other.command;
            index = other.index;
            value = other.value;
            DataAppend = other.DataAppend;
            return *this;
        }

        void setAppendCenter(const Vec3D &scenter)
        {
            DataAppend.setCenter(scenter);
        }

        void setAppendBiLine(const BiLine &sbiline)
        {
            DataAppend.setBiLine(sbiline);
        }



    };

    typedef vector<AnimCommand> ANIMcommands;
    typedef ANIMcommands::iterator ANIMCMDit;


    struct RPDynamic
    {
        Balls  balls;
        Tracks cursors;
        VerticesGrp links;

        VerticesGrp contours;

        RPDynamic()
        {
            clear();
        }

        void clear()
        {
            contours.clear();
            links.clear();
            balls.clear();
            cursors.clear();
        }

        bool buildContour(DataRP::RPcontour &scontour, double curZ)
        {
            RPpolygon::iterator itver;
            RPVertices loop;
            //RPlines2D linelist;
            for(itver = scontour.vertList.begin(); itver != scontour.vertList.end(); itver++)
            {
               loop.push_back(Vec3D(itver->getX(),itver->getY(),curZ));
            }

            if(loop.size() < 3)
                return false;

            contours.push_back(loop);
            return true;
        }

        //build balls' link.
        bool buildLinks(unsigned start, unsigned send)
        {
            RPVertices link;

            if(send == 0 )
                return false;

            RPDisk ball,ballFirst;
            bool bstart = true;

            for(unsigned i = start; i < send; i++)
            {

                ball = balls[i];
                if(ball.getStatus() != DR_BALL_DELETED )//DR_BALL_DELETED
                {
                    link.push_back(ball.getCenter());

                    if(bstart)
                    {
                        ballFirst = ball;
                        bstart = false;
                    }

                }//if
            }//for

            link.push_back(ballFirst.getCenter());

            if(link.size() < 3)
                return false;

            links.push_back(link);

            return true;
         }
    };

    class SimuBallRoll
    {

    public:
        ANIMcommands m_acommds;
        RPDynamic m_animData;

        SimuBallRoll():b_active(false), b_stepOn(false)
        {
            clear();
        }

        ~SimuBallRoll(){}

        void clear()
        {
            m_start = 0;
            m_end = m_start;
            m_acommds.clear();
            m_animData.clear();

        }


        void addBall(RollBall rollball, double sz)
        {
            if(!b_active)
                return;

            Vec3D v3center;
            RPDisk aniball;

            v3center = Vec3D(rollball.center.getX(), rollball.center.getY(), sz);
            aniball = RPDisk(v3center,rollball.radius);
            push_ball(aniball);
        }

        void addCursor(RPline2D sline,double sz,double cursorSize)
        {
            if(!b_active)
                return;

            Line v3line = sline.line23D(sz);
            push_cursor(v3line.getCursor(cursorSize));
        }

        void addBallsOK(unsigned slimit)
        {

            if(!b_active)
                return;

            m_start = m_end;
            m_end = m_animData.balls.size();
            push_command(AnimCommand(ANP_SHOW_BALLS,slimit));
        }

        void deleteBall(unsigned cursor)
        {
            if(!b_active)
                return;

            push_command(AnimCommand(ANP_DELETE_BALL,cursor));
        }

        void moveBall(RollBall rollball,unsigned cursor,RPline2D uline, RPline2D vline,double sz)
        {
            if(!b_active)
                return;

            Vec3D v3center;
            Line u3line,v3line;
            AnimCommand ancommd;
            BiLine biline;

            u3line = uline.line23D(sz);
            v3line = vline.line23D(sz);
            biline = BiLine(u3line,v3line);
            ancommd = AnimCommand(ANP_HELP_LINE,0);
            ancommd.setAppendBiLine(biline);
            m_acommds.push_back(ancommd);

            ancommd.clear();
            v3center = Vec3D(rollball.center.getX(), rollball.center.getY(), sz);
            ancommd = AnimCommand(ANP_MOVE_BALL,cursor);
            ancommd.setAppendCenter(v3center);
            m_acommds.push_back(ancommd);
        }

        void updateCursor(unsigned cursor, unsigned precursor)
        {
            if(!b_active)
                return;

            push_command(AnimCommand(ANP_UPDATE_CURSOR,cursor,precursor));
        }

        void addContactLines(Tracks &contactlines)
        {
            if(!b_active)
                return;

            AnimCommand ancommd;
            ancommd = AnimCommand(ANP_CONTACT_LINES,0);
            std::copy(contactlines.begin(),contactlines.end(),std::back_inserter(ancommd.DataAppend.contactLines));
            push_command(ancommd);
        }

        void addContour(DataRP::RPcontour &scontour, double curZ)
        {
            if(!b_active)
                return;
            push_command(AnimCommand(ANP_CONTOUR_LOOP,0));
            m_animData.buildContour(scontour,curZ);
        }

        void endOneLoop()
        {
            if(!b_active)
                return;

            push_command(AnimCommand(ANP_FINISH_LOOP,m_start,m_end));
        }

        void setActive(bool ba)
        {
            b_active = ba;
        }

        void goOn(bool bstep)
        {
            b_stepOn = bstep;
        }

        bool isStepOn()
        {
            return b_stepOn;
        }

        unsigned getCommdsSize()
        {
            return m_acommds.size();
        }

    protected:

        void push_command(AnimCommand commd)
        {
            m_acommds.push_back(commd);
        }

        void push_ball(RPDisk &ball)
        {
            m_animData.balls.push_back(ball);
        }

        void push_cursor(Line cursor)
        {
            m_animData.cursors.push_back(cursor);
        }

   private:

        unsigned m_start,m_end;
        bool b_active;
        bool b_stepOn;

    };
}

#endif // DATADYNAMIC_H
