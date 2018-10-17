/***************************************************************************
                          rpexport.h  -  description
                             -------------------
    begin                : Nov. 5 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef RPEXPORT_IN
#define RPEXPORT_IN

#include "rpplugin.h"


// Return the name of the plugin - note that we do not user class members as
// the class may not yet be insantiated when this method is called.
    RPEXPORT QString name()
    {
      return RPPlugin::name();
    }

    // Return the description
    RPEXPORT QString description()
    {
      return RPPlugin::description();
    }

    // Return the category
    RPEXPORT int category()
    {
      return RPPlugin::category();
    }

    // Return the version number for the plugin
    RPEXPORT QString version()
    {
      return RPPlugin::version();
    }

    RPEXPORT QString icon()
    {
      return RPPlugin::icon();
    }

#endif // RPEXPORT_IN
