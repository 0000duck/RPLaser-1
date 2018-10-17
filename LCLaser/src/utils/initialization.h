/***************************************************************************
                          initialization.h  -  description
                             -------------------
    begin                : Jun. 23 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <QSplashScreen>

class RPInitiate: public QObject
{
     Q_OBJECT
public:
    RPInitiate(QSplashScreen *parent = NULL);
    virtual ~RPInitiate();

    void getConfig();
    void recordConfig();


private:

    QSplashScreen *m_parent;
};


#endif // INITIALIZATION_H
