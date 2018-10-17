/***************************************************************************
                          simuthread.h  -  description
                             -------------------
    begin                : Jun. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef SIMUTHREAD_H
#define SIMUTHREAD_H

#include <QThread>
#include <simu.h>
#include "control.h"


namespace SIMURP
{

    class SimuThread:public QThread
    {
        Q_OBJECT
    public:
        explicit SimuThread(RPsimu *source, QObject *parent = 0);
        virtual ~SimuThread();

    signals:
        void resultReady(const QString &str);


    protected:
        void run() Q_DECL_OVERRIDE;

    private:
        RPsimu *rp_simu;
    };

}
#endif // SIMUTHREAD_H
