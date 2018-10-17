/***************************************************************************
                          singleprint.h  -  description
                             -------------------
    begin                : Nov. 21 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#ifndef SINGLE_PRINT_H
#define SINGLE_PRINT_H

#include "rpexport.h"
#include "singleprintwidget.h"


class SinglePrint : public RPPlugin
{
public:
    SinglePrint(RPInterface *interface);
    ~SinglePrint();

    virtual void initGui();
    virtual void unload();
};



#endif // SINGLE_PRINT_H
