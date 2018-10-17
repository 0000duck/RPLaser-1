/***************************************************************************
                          buffthread.h  -  description
                             -------------------
    begin                : Jun. 16 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef BUFF_THREAD_H
#define BUFF_THREAD_H

#include <QThread>
#include "control.h"

typedef void process(BuildRP::RPControl*);

namespace BuildRP
{

    class BuffThread:public QThread
    {
        Q_OBJECT
    public:
        explicit BuffThread(QObject *parent = 0);
        virtual ~BuffThread();
        void setProcess(process *sp,BuildRP::RPControl *rpctl,QString str);

    signals:
        void resultReady(QString str);


    protected:
        void run() Q_DECL_OVERRIDE;

    private:
        RPControl *rp_control;
        process *m_process;
        QString stReady;
    };
}

#endif//BUFF_THREAD_H
