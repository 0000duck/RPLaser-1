/***************************************************************************
                          datathread.h  -  description
                             -------------------
    begin                : Sep. 23 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin

    description          :can be update to DataprocessThread.   just convert HandelScanPath to it's parent
                          parent can produce many child class.
 ***************************************************************************/
#ifndef DATA_THREAD_H_
#define DATA_THREAD_H_

#include <QThread>
#include "handelscanpath.h"

typedef void process(DataRP::HandelScanPath*);

namespace DataRP
{

    class DataThread:public QThread
    {
        Q_OBJECT
    public:
        explicit DataThread(QObject *parent = 0);
        virtual ~DataThread();
        void setProcess(process *sp,DataRP::HandelScanPath *handelline,QString str);

    signals:
        void resultReady(QString str);


    protected:
        void run() Q_DECL_OVERRIDE;

    private:
        DataRP::HandelScanPath *handle_line;
        process *m_process;
        QString stReady;
    };
}

#endif//DATA_THREAD_H_
