/***************************************************************************
                          threadlog.h  -  description
                             -------------------
    begin                : Nov. 19 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef THREAD_LOG_H
#define THREAD_LOG_H

#include <fstream>
#include <QDir>

class ThreadLog
{
public:
    static void  logging(QString content)
    {
        QString fileName = QDir::currentPath()+"/../share/threadlog.txt";
        QByteArray ba = fileName.toLocal8Bit();
        const char *path = ba.data();

        ofstream log(path, ios::app);

        if(!log.is_open())
        {
            return;
        }

        ba = content.toLocal8Bit();
        log <<ba.data();
        log.close();
    }
};


#endif // THREAD_LOG_H
