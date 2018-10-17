/***************************************************************************
                          datainterface.h  -  description
                             -------------------
    begin                : Nov. 23 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DATA_INTERFACE__H
#define DATA_INTERFACE__H

#include <QObject>

namespace DataRP
{
    class DataInterface:public QObject
    {

    public:
        explicit DataInterface(QObject *parent = 0)
            :QObject(parent)
        {
        }

        virtual ~DataInterface()
        {
        }

    };
}

#endif // DATA_INTERFACE__H
