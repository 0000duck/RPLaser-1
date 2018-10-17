/***************************************************************************
                          simu.h  -  description
                             -------------------
    begin                : May 20 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef RPSIMU_H
#define RPSIMU_H

#include <QObject>
#include <QTime>
#include "data.h"
#include "QBasicTimer"

namespace SIMURP
{

    class RPsimu : public QObject
    {
        Q_OBJECT
      public:
            explicit RPsimu(QObject *parent = 0);
            RPsimu(DataRP::RPData *rpdata, QObject *parent = 0);
            ~RPsimu();
            virtual void timeConsuming() = 0;

      protected:

            DataRP::RPData *rp_data;
            QBasicTimer m_timer;
            double  m_frameInterval;
    };
}
#endif // RPSIMU_H
