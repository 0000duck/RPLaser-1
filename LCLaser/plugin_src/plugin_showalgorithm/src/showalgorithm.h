/***************************************************************************
                          showalgorithm.h  -  description
                             -------------------
    begin                : Nov. 21 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#ifndef EXPORT_STL_PLUGIN_H
#define EXPORT_STL_PLUGIN_H

#include "rpexport.h"

class ShowAlgorithm:  public RPPlugin
{

public:
    ShowAlgorithm(RPInterface *interface);
    ~ShowAlgorithm();

    virtual void initGui();
    virtual void unload();
};




#endif // EXPORT_STL_PLUGIN_H
