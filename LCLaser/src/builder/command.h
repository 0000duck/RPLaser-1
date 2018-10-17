/***************************************************************************
                          command.h  -  description
                             -------------------
    begin                : Jun. 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef CONTROL_COMMAND_H
#define CONTROL_COMMAND_H

#include <iostream>
#include <list>
#include "Vec3D.h"

using namespace std;

struct RPSpeed
{
    double velocity;
    double acceleration;


    RPSpeed():velocity(100),acceleration(0.5)
    {
    }

    RPSpeed(double vel, double acc)
    {
        velocity = vel;
        acceleration = acc;
    }

    RPSpeed& operator=(RPSpeed &other)
    {
        velocity = other.velocity;
        acceleration = other.acceleration;
        return *this;
    }
};

struct SpeedSet
{
    RPSpeed moveSpeed;
    RPSpeed fillingSpeed;
    RPSpeed outlineSpeed;
    SpeedSet()
    {}
    //move
    void setMoveVel(double sv)
    {
        moveSpeed.velocity = sv;
    }

    void setMoveAcc(double sv)
    {
        moveSpeed.acceleration = sv;
    }

    //filling
    void setFillingVel(double sv)
    {
        fillingSpeed.velocity = sv;
    }

    void setFillingAcc(double sv)
    {
        fillingSpeed.acceleration = sv;
    }

    //outline
    void setOutlineVel(double sv)
    {
        outlineSpeed.velocity = sv;
    }

    void setOutlineAcc(double sv)
    {
         outlineSpeed.acceleration = sv;
    }


    SpeedSet &operator=(SpeedSet &other)
    {
        moveSpeed = other.moveSpeed;
        outlineSpeed = other.outlineSpeed;
        return *this;
    }

    double getMoveVel()
    {
        return moveSpeed.velocity;
    }

    double getMoveAcc()
    {
        return moveSpeed.acceleration;
    }

    double getFillingVel()
    {
        return fillingSpeed.velocity;
    }

    double getFillingAcc()
    {
        return fillingSpeed.acceleration;
    }

    double getOutlineVel()
    {
        return outlineSpeed.velocity;
    }

    double getOutlineAcc()
    {
        return outlineSpeed.acceleration;
    }
};

namespace BuildRP
{
    enum CommandName
    {
        CT_NULL = 0,
        CT_SetSynVel,
        CT_BuffOn,
        CT_LnXYZG0,
        CT_LnXY,
        CT_LnXYZ,
        CT_ArcXY,
        CT_ArcXYP,
        CT_BufIOBit,
        CT_Wait,
        CT_OVERLAYER,
        CT_Finish,
    };


    struct RPCommd
    {
        int commdname;
        double component[6];
        //!
        //! \constructor.
        //!
        RPCommd()
        {
            clear();
        }

        RPCommd(int commd, double sx, double sy, double sz, double sa =0, double sb = 0, double sc=0)
        {
            commdname = commd;
            component[0] = sx;
            component[1] = sy;
            component[2] = sz;
            component[3] = sa;
            component[4] = sb;
            component[5] = sc;
        }

        /***************setting*********************/
        void clear()
        {
             for(int i = 0 ;i < 6 ; i++)
                component[i] = 0;

             commdname = 0;
        }


        void setCommand(const int &commd)
        {
            commdname = commd;
        }

        void setArcXYP(double sx, double sy, double radius, double direction, double cx, double cy)
        {
            component[0] = sx;
            component[1] = sy;
            component[2] = radius;
            component[3] = direction;
            component[4] = cx;
            component[5] = cy;
        }

         void setArcXY(double cx, double cy, double radian, double radius,double sx, double sy)
        {
            component[0] = cx;
            component[1] = cy;
            component[2] = radian;
            component[3] = radius;
            component[4] = sx;
            component[5] = sy;

        }

        void setLnXYZ(double sx, double sy, double sz)
        {
            component[0] = sx;
            component[1] = sy;
            component[2] = sz;
        }

        void setLnXY(double sx, double sy)
        {
            component[0] = sx;
            component[1] = sy;
        }

        void setLnXYZ(const Vec3D &point)
        {
            component[0] = point.getX();
            component[1] = point.getY();
            component[2] = point.getZ();
        }

        void setStatusValue(double sx, double sy)
        {
            component[0] = sx;
            component[1] = sy;
            component[2] = 0.0;
            component[3] = 0.0;
        }

        void setsynVel(double sv)
        {
            component[0] = sv;
        }

        /***************getting*********************/
        int getCommand()
        {
            return commdname;
        }

         void getValue(double &tx, double &ty, double &tz, double &ta,double &tb,double &tc)
        {
            tx = component[0];
            ty = component[1];
            tz = component[2];
            ta = component[3];
            tb = component[4];
            tc = component[5];
        }

        void getValue(double &tx, double &ty, double &tz, double &ta)
        {
            tx = component[0];
            ty = component[1];
            tz = component[2];
            ta = component[3];
        }

        void getValue(double &tx, double &ty, double &tz)
        {
            tx = component[0];
            ty = component[1];
            tz = component[2];
        }

        void getValue(double &tx, double &ty)
        {
            tx = component[0];
            ty = component[1];
        }

        void getValue(double &sv)
        {
            sv = component[0];
        }

        Vec3D getValue()
        {
            Vec3D vec;
            vec.setX(component[0]);
            vec.setY(component[1]);
            vec.setZ(component[2]);
            return vec;
        }

        /***************operator*********************/
        RPCommd& operator =(const RPCommd &other)
        {
            commdname = other.commdname;
            component[0] = other.component[0];
            component[1] = other.component[1];
            component[2] = other.component[2];
            component[3] = other.component[3];
            component[4] = other.component[4];
            component[5] = other.component[5];
            return *this;
        }

    };

       typedef list<RPCommd> CommdList;
}

#endif//CONTROL_COMMAND_H
