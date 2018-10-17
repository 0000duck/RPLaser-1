/*******************************************************************************
Copyright (c) 2014, XI'AN JIAOTONG UNIVERSITY
Author Cui Bin
This file is part of the RPmachine Tools suite.
*******************************************************************************/

#ifndef _VEC2D_H
#define _VEC2D_H

#include "Vec3D.h"
#include <QDebug>

class Vec2D
{
public:

   double x,y;

    Vec2D():x(0),y(0){}

    Vec2D(double sx, double sy):x(sx),y(sy){}

    Vec2D(const Vec2D &other){ x=other.x;  y=other.y;}

    double getX() const { return x;}

    double getY() const { return y;}

    void setX(const double sx){ x = sx;}

    void setY(const double sy){ y = sy;}

     Vec3D toVec3D(double tz)
     {
        Vec3D point = Vec3D(getX(),getY(),tz);
        return point;
     }

    friend Vec2D operator*(const Vec2D& v, const double f) {return Vec2D(v.x*f, v.y*f);}
    friend Vec2D operator*(const double f, const Vec2D& v) {return v*f;}
    friend Vec2D operator-(const Vec2D& v1, const Vec2D& v2){return Vec2D(v1.x-v2.x, v1.y-v2.y); }
    friend Vec2D operator+(const Vec2D& v1, const Vec2D& v2){return Vec2D(v1.x+v2.x, v1.y+v2.y); }
           Vec2D& operator+=(const Vec2D& s) {x += s.x; y += s.y; return *this;}
    friend bool operator==(const Vec2D& v1, const Vec2D& v2) {return (v1.x==v2.x && v1.y==v2.y);}
    friend bool operator!=(const Vec2D& v1, const Vec2D& v2) {return !(v1==v2);}
    inline Vec2D& operator=(const Vec2D& s){x = s.x; y = s.y;return *this;}
    inline double length() { return sqrt(x*x + y*y); }
    inline double Normalize() { double len = length(); if(len > 0) {  x /= len;  y /= len; } return len; }
    inline double Dot(const Vec2D& v) {return (x * v.x + y * v.y);}
    inline double Cross(const Vec2D& v) {return x*v.y-y*v.x;}
    inline Vec2D VerticalXYLeft(){ this->Normalize();return Vec2D(-y,x);}
    inline Vec2D VerticalXYRight(){this->Normalize();return Vec2D(y,-x);}
    void RotZ(double a) {double xt =  x*cos(a) - y*sin(a); double yt = x*sin(a) + y*cos(a); x = xt; y = yt;}

    inline bool IsApproximateEqual(const Vec2D &v, double times)
    {
        double coef = pow(10, times);
        double sx,sy,tx,ty;
        sx = floor(x*coef)/coef;   sy = floor(y*coef)/coef;
        tx = floor(v.x*coef)/coef; ty = floor(v.y*coef)/coef;
        return(sx==tx && sy == ty);
    }

};

namespace DataRP
{
    enum DirectType
    {
       DIRECT_LEFT,
       DIRECT_RIGHT,
    };

    enum BumpType
    {
       BUMP_CONVEX = 0,
       BUMP_CONCAVE,
    };

    class RPpoint2D: public Vec2D
       {
         public:
           RPpoint2D():Vec2D(),b_erase(false)
           {}

           RPpoint2D(double sx, double sy,bool berase=false):Vec2D(sx,sy),b_erase(berase)
           {}

           RPpoint2D& operator=(const RPpoint2D& s){x = s.x; y = s.y;b_erase = s.b_erase;return *this;}

           void setErase(bool mark= true){ b_erase = mark; }

           bool isErase() {  return b_erase; }

         private:
           bool b_erase;
       };

    struct RPline2D
    {
        Vec2D lhead;
        Vec2D ltail;
        Vec2D normal;

        RPline2D()
        {
           lhead = Vec2D(1.0,0.0);
           ltail = Vec2D(0.0,1.0);
           setNormal();
        }

        RPline2D(const Vec2D &head,const Vec2D &tail)
        {
           lhead = head;
           ltail = tail;
           setNormal();
        }

        void setHead(const Vec2D &head)
        {
           lhead = head;
        }

        void setTail(const Vec2D &tail)
        {
           ltail = tail;
        }

        Vec2D getHead() const
        {
           return lhead;
        }

        Vec2D getTail() const
        {
           return ltail;
        }

        void setHead(const Vec2D &head, Vec3D &ray,double dist)
        {
           lhead = head;
           Vec2D lray = Vec2D(ray.getX(),ray.getY());
           lhead += lray*dist;
        }

        void setTail(const Vec2D &tail,Vec3D &ray,double dist)
        {
           ltail = tail;
           Vec2D lray = Vec2D(ray.getX(),ray.getY());
           ltail += lray*dist;
        }

        void setNormal()
        {
            normal = ltail - lhead;
            normal.Normalize();
        }

         Vec2D getVector() const
        {
           return normal;
        }

        double getLength()
        {
            Vec2D vec = ltail - lhead;
            return vec.length();
        }

        double distance2Point(const Vec2D &point)
        {
            setNormal();
            Vec2D lstake = normal.VerticalXYLeft();
            Vec2D tail2Pointvec = ltail - point;
            return fabs(tail2Pointvec.Dot(lstake));
        }

       //return intersce point.
        Vec2D intersect(const RPline2D &other)
        {
            //is tow line parallel.
            if (normal.Cross(other.normal) == 0 )
            {
                qDebug()<<"line intersect parallel"<<endl;
                return other.ltail;
            }

            Vec2D terminalVec = other.ltail - lhead;
            terminalVec.Normalize();

            double dist = distance2Point(other.ltail);
            double length = dist/sqrt(1 - normal.Dot(other.normal)*normal.Dot(other.normal));

            if(normal.Cross(other.normal) > 0)
            {
               if(normal.Cross(terminalVec) > 0 )
                   return other.ltail - (other.normal * length);
               else
                   return other.ltail + (other.normal * length);
            }
            else
            {

               if(normal.Cross(terminalVec) > 0 )
                   return other.ltail + (other.normal * length);
               else
                   return other.ltail - (other.normal * length);
            }
       }

        void translate(const Vec2D &vec, double length)
        {
           lhead = lhead + vec *length;
           ltail = ltail + vec *length;
           setNormal();
        }

        //end name 0: middle, 1: head, 2:tail.
        double Point2line(Vec2D point, Vec2D &intersecpoint, int &endname)
        {
           double lside,distance,lenhead,lentail,vcos, vsin;
           Vec2D lstake,tail2Pointvec;

           Vec2D tail2Point = point -ltail;
           Vec2D head2Point = point -lhead;

           //vertical distance.
           lstake = normal.VerticalXYLeft();
           distance = fabs(tail2Point.Dot(lstake));

           lentail= tail2Point.length();
           tail2Pointvec = tail2Point;
           tail2Pointvec.Normalize();

           vcos = tail2Pointvec.Dot(lstake);
           vsin = sqrt(1 - vcos*vcos);
           lside = lentail * vsin;


           if(tail2Pointvec.Dot(normal) < 0)
                intersecpoint = ltail - lside*normal;
           else
                 intersecpoint = ltail + lside*normal;

           if(isPointOnLine(intersecpoint))
           {
                endname = 3;
                return distance;
           }

           lenhead = head2Point.length();

           if(lenhead < lentail)
           {
                distance = lenhead;
                endname = 1;
           }
           else
           {
                distance = lentail;
                endname = 2;
           }
            return distance;
        }

         bool isPointOnLine(const Vec2D &spoint,int &status)
        {
            double cross;
            status = 3;

            Vec2D tail2point = spoint - ltail;
            Vec2D head2point = spoint - lhead;

            if(head2point.length() < 0.001)
            {
              status = 1;
              return true;
            }

             if(tail2point.length() < 0.001)
            {
               status = 2;
              return true;
            }

            tail2point.Normalize();
            setNormal();

            cross = tail2point.getX()*normal.getY() - tail2point.getY()*normal.getX();
            //delete some deviate.
            if(fabs(cross) > 0.001)
            {
                return false;
            }

            head2point.Normalize();

            if(head2point.Dot(tail2point) < 0)
            {
                return true;
            }
                return false;
        }

        bool isPointOnLine(const Vec2D &spoint)
        {
            double cross;

            Vec2D tail2point = spoint - ltail;
            Vec2D head2point = spoint - lhead;

            if(head2point.length() < 0.001)
            {
                 return true;
            }

            if(tail2point.length() < 0.001)
            {
                return true;
            }

            tail2point.Normalize();
            setNormal();

            cross = tail2point.getX()*normal.getY() - tail2point.getY()*normal.getX();
            //delete some deviate.
            if(fabs(cross) > 0.001)
            {
                return false;
            }


            head2point.Normalize();

            if(head2point.Dot(tail2point) < 0)
            {
                return true;
            }
                return false;
        }

        Line line23D(double tz)
        {
            Vec3D thead = Vec3D(lhead.getX(),lhead.getY(),tz);
            Vec3D ttail = Vec3D(ltail.getX(),ltail.getY(),tz);
            Line line3D = Line(thead,ttail);
            return line3D;
        }

        double Dot(const RPline2D &other)
        {
           return this->getVector().Dot(other.getVector());
        }

        double Cross(const RPline2D &other)
        {
           return this->getVector().Cross(other.getVector());
        }

        RPline2D& operator=(const RPline2D& other)
        {
            this->setHead(other.getHead());
            this->setTail(other.getTail());
            this->setNormal();
            return *this;
        }

    };

    class RP2DDisk
    {

    private:

        Vec2D center;
        double radius;
        int status;

    public:

        RP2DDisk():center(Vec2D()),radius(1),status(0)
        {}

        RP2DDisk(const Vec2D &scenter,double sradius):status(0)
        {
            center = scenter;
            radius = sradius;
        }

        //! from startPos to endPos, if radius = -1 major arc.
        RP2DDisk(const Vec2D &startPos,const Vec2D &endPos,double sradius):status(0)
        {
            if(sradius == -1)
               radius = -sradius;
            else
               radius =  sradius;


            Vec2D vec = endPos - startPos;
            double len = vec.length()/2.0;
            vec.Normalize();

            if(len == radius)
            {
                center = startPos + vec*radius;
                return;
            }


            double cosine = len/radius;
            double theta = acos(cosine);

            if(sradius < 0)
            {
                vec.RotZ(theta);
            }
            else
            {
                vec.RotZ(-theta);
            }
            center = startPos + vec*radius;
        }


        void setStatus(int sst)
        {
            status = sst;
        }

        int getStatus() const
        {
            return status;
        }

        void setRadius(double sradius)
        {
            radius = sradius;
        }

        double getRadius() const
        {
            return radius;
        }

        void setCenter(const Vec2D &scenter)
        {
            center = scenter;
        }

        Vec2D getCenter() const
        {
            return center;
        }

        //! line1 and lin2 must head to end on same direction.
        Vec2D getCenter(const Vec2D shead, const Vec2D smid,const Vec2D send, bool &concave,Vec2D &intersec1, Vec2D &intersec2)
        {
            double sinangle = 0;
            RPline2D line[2];
            Vec2D lvector[2],intersecPoint,v0left,v1left,vrot;
            double vcos,theta,angle,longside,wingside,clockwise;
            line[0] = RPline2D(shead,smid);
            line[1] = RPline2D(smid,send);
            //get line intersection point.
            intersecPoint = smid;
            //normalize lines.
            for(int i = 0; i < 2; i ++)
            {
              line[i].setNormal();
              lvector[i] = line[i].getVector();
            }

            //calculate angle and length.
            v0left = lvector[0].VerticalXYLeft();
            v1left = lvector[1].VerticalXYLeft();
            //rotting direction, centerly >0, because vector dot it's leftvector.
            //clockwise = lvector[1].getX()*vleft.getY() - lvector[1].getY()*vleft.getX();
            clockwise = v0left.getX()*v1left.getY() - v0left.getY()*v1left.getX();


            //two line are  parallel.
            if(clockwise == 0)
            {
                 if(shead == send)  //two line overlap.
                {
                    qDebug()<<"two line are overlapped"<<endl;
                }

                concave = true;
                intersec1 = smid;
                intersec2 = smid;
                return smid + v0left * radius;
            }
            //actual intersection angle. pi - x
            vcos = -lvector[0].Dot(lvector[1]);
            if(vcos > 1)
                vcos = 1;

            if(vcos < -1)
                vcos =-1;

            angle = (acos(vcos))/2;
            //orgin intersection angle.
            vcos =  -vcos;
            theta = (acos(vcos));
            //sides.

            sinangle = (sin(angle));

            if(sinangle == 0 )
                longside = radius;
            else
                longside = radius/sinangle;

            wingside = longside*cos(angle);
            //rotate second line to find the direction to center.
            vrot = lvector[1];

            if(clockwise > 0)
            {
               vrot.RotZ(angle);
               center = intersecPoint + vrot*longside;
               concave = false;
            }
            else
            {
               vrot.RotZ(theta + angle);
               //center = intersecPoint + vrot*radius;
               center = intersecPoint + vrot*longside;
               concave = true;
            }

            //calculate results.

            //search intresection point.
            if(clockwise > 0)
            {
             intersec1 = intersecPoint - lvector[0]*wingside;
             intersec2 = intersecPoint + lvector[1]*wingside;
            }
            else
            {
             intersec1 = intersecPoint;
             intersec2 = intersecPoint;
            }

#ifdef WIN32
            if(_isnan(center.getX())|| !_finite(center.getX())||
               _isnan(center.getY())|| !_finite(center.getY()))
#else
            if(isinf(center.getX())|| isnan(center.getX())||
               isinf(center.getY())|| isnan(center.getY()))
#endif
            {
                qDebug()<<"get ball center error"<<endl;
            }
            return center;
        }


         //! line1 and lin2 must head to end on same direction.
        Vec2D getInternalCenter(const Vec2D shead, const Vec2D smid,const Vec2D send,Vec2D &intersec1, Vec2D &intersec2,Vec2D &vrot)
        {
            double sinangle = 0;
            RPline2D line[2];
            Vec2D lvector[2],intersecPoint,v0left;
            double vcos,angle,longside,wingside,clockwise;
            line[0] = RPline2D(shead,smid);
            line[1] = RPline2D(smid,send);
            //get line intersection point.
            intersecPoint = smid;
            //normalize lines.
            for(int i = 0; i < 2; i ++)
            {
              line[i].setNormal();
              lvector[i] = line[i].getVector();
            }

            v0left = lvector[0].VerticalXYLeft();

            clockwise = line[0].Cross(line[1]);

            //two line are  parallel.
            if(clockwise == 0)
            {
                 if(shead == send)  //two line overlap.
                {
                    qDebug()<<"two line are overlapped"<<endl;
                }
                return smid + v0left * radius;
            }
            //actual intersection angle. pi - x
            vcos = -lvector[0].Dot(lvector[1]);
            if(vcos > 1)
                vcos = 1;

            if(vcos < -1)
                vcos =-1;

            angle = (acos(vcos))/2;

            //sides.
            sinangle = (sin(angle));

            if(sinangle == 0 )
                longside = radius;
            else
                longside = radius/sinangle;

            wingside = longside*cos(angle);

            //rotate second line to find the direction to center.
            vrot = lvector[1];

            if(clockwise > 0)
            {
                vrot.RotZ(angle);
                center = intersecPoint + vrot*longside;
            }
            else
            {
                vrot.RotZ(-angle);
                center = intersecPoint + vrot*longside;
            }

            //search intresection point.
            intersec1 = intersecPoint - lvector[0]*wingside;
            intersec2 = intersecPoint + lvector[1]*wingside;

#ifdef WIN32
            if(_isnan(center.getX())|| !_finite(center.getX())||
               _isnan(center.getY())|| !_finite(center.getY()))
#else
            if(isinf(center.getX())|| isnan(center.getX())||
               isinf(center.getY())|| isnan(center.getY()))
#endif
            {
                qDebug()<<"get ball center error"<<endl;
            }
            return center;
        }


        //cannot generator ball -1, concave 1, convex 0.
        int getCenter(const RPline2D &line1, const RPline2D &line2, Vec2D &center, Vec2D &intersec1,Vec2D &intersec2)
        {
            if(line2.getHead() != line1.getTail())
            {
                return -1;
            }

            bool isconcave = false;
            Vec2D lhead = line1.getHead();
            Vec2D lmid  = line1.getTail();
            Vec2D ltail = line2.getTail();

            center = getCenter(lhead, lmid, ltail, isconcave,intersec1, intersec2);

            if(isconcave)
               return 1;

            return 0;
        }

          //! is disk intersected with the line.
        bool isIntersect(const RPline2D &sline)
        {
            double distcenter;
            Vec2D intersecpoint;
            int endname = 0;
            RPline2D line = sline;

            distcenter = line.Point2line(center, intersecpoint, endname);

            //the line is not contact disk.
            if(distcenter >= radius)
            {
                return false;
            }

            return true;
        }

           //! is disk intersected with the line.
        bool isIntersect(const RPline2D &sline, int &endname, Vec2D &intsecPos)
        {
            double distcenter;
            RPline2D line = sline;

            distcenter = line.Point2line(center, intsecPos, endname);

            //the line is not contact disk.
            if(distcenter >= radius)
            {
                return false;
            }

            return true;
        }
    };
}//namespace

#endif //_VEC3D_H
