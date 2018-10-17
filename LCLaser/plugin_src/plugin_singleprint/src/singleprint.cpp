/***************************************************************************
                          singleprint.cpp  -  description
                             -------------------
    begin                : Nov. 21 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#include <QHBoxLayout>
#include "singleprint.h"


const QString RPPlugin::mName = QObject::tr( "single line print" );
const QString RPPlugin::mDescription = QObject::tr( "single line or circle printing testing" );
const int RPPlugin::mCategory = CATAGORY_TEST;
const QString RPPlugin::mVersion = QObject::tr( "Version 0.1" );
const QString RPPlugin::mIcon = ":/singleprint.png";

SinglePrint::SinglePrint(RPInterface *interface):
RPPlugin(interface)
{
    initGui();
}

SinglePrint::~SinglePrint()
{

}

void SinglePrint::initGui()
{
    m_widget = new SinglePrintWidget(m_interface->iface_control);
    m_layout=new QHBoxLayout(NULL);
    m_layout->addWidget(m_widget);
    m_widget->show();
    m_interface->addPluginOnControlTab(this);
}

void SinglePrint::unload()
{
    unloadWidget();
    unloadlayout();
}

RPEXPORT RPPlugin * classFactory( RPInterface * interfacePointer )
{
  return new SinglePrint( interfacePointer );
}




