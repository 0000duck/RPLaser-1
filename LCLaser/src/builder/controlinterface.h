/***************************************************************************
                          controlinterface.h  -  description
                             -------------------
    begin                : Nov. 23 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef CONTROL_INTERFACE__H
#define CONTROL_INTERFACE__H

#include <QObject>

namespace BuildRP
{
    class ControlInterface:public QObject
    {

    public:
        explicit ControlInterface(QObject *parent = 0)
            :QObject(parent)
        {
        }

        virtual ~ControlInterface()
        {
        }

    public:
        virtual void setSingleTrack(int trackType) = 0;
        virtual void setSingleHeight(int smaxHeight) = 0;
        virtual void setSingleLenghtRadius(double svalue) = 0;
        virtual void SetSingleDeltaZ(double sdZ) = 0;
        virtual void singleBuffOn() = 0;
        virtual void singleBuffOff() = 0;
        virtual void singleBuffContinue() = 0;
        virtual void SingleClearBuff() = 0;
    };
}

#endif // CONTROL_INTERFACE__H
