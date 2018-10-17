/***************************************************************************
                          ioslc.h  -  description
                             -------------------
    begin                : Apr. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef IOSIC_H
#define IOSIC_H

#include "QString"
#include "QFile"
#include "database.h"
#include "data.h"

namespace IORP
{
    class RPIOslc
    {
    public:
        RPIOslc();
        ~RPIOslc();
        void openFile(QString fileName,DataRP::RPData *sslc);

    protected:
        bool readSlcHeader();
        bool readSlcData();

    private:
        QString readSlcHeaderItem(QString keyword, QByteArray &line);
        QString m_filename;
        unsigned m_current;
        DataRP::RPData *slc;
    };
}

#endif // IOSIC_H
