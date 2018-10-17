/***************************************************************************
                          showalgorithm.cpp  -  description
                             -------------------
    begin                : Nov. 21 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "showalgorithm.h"


const QString RPPlugin::mName = QObject::tr( "show ball algorithm" );
const QString RPPlugin::mDescription = QObject::tr( "ball algorithm for generating outlines" );
const int RPPlugin::mCategory = CATAGORY_DEBUG;
const QString RPPlugin::mVersion = QObject::tr( "Version 0.1" );
const QString RPPlugin::mIcon = ":/ballalgorithm.png";

ShowAlgorithm::ShowAlgorithm(RPInterface *interface):
RPPlugin(interface)
{
    initGui();
}

ShowAlgorithm::~ShowAlgorithm()
{

}

void ShowAlgorithm::initGui()
{
    m_interface->showAlogrithm(true);
}

void ShowAlgorithm::unload()
{
    m_interface->showAlogrithm(false);
}

RPEXPORT RPPlugin * classFactory( RPInterface * interfacePointer )
{
  return new ShowAlgorithm( interfacePointer );
}




