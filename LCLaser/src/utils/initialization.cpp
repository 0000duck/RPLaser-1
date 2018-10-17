/***************************************************************************
                          initialization.cpp  -  description
                             -------------------
    begin                : Jun. 23 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include "initialization.h"
#include "config.h"

RPInitiate::RPInitiate( QSplashScreen *parent):m_parent(parent)
{
}

RPInitiate::~RPInitiate()
{
}


void RPInitiate::getConfig()
{
    //read config
    RPConfig rpconfig;
    QString configName = QDir::currentPath()+"/../share/config.xml";
    QFile file(configName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(m_parent, tr("config file"), tr("Cannot read file %1:\n%2").arg(configName).arg(file.errorString()));
        return;
    }

   if(rpconfig.readConfig(&file))
   {
       m_parent->showMessage(tr("config loaded OK"),Qt::AlignLeft|Qt::AlignBottom,Qt::blue);
   }
   file.close();
}

void RPInitiate::recordConfig()
{
    RPConfig rpconfig;
    QString configName  = QDir::currentPath()+"/../share/config.xml";

    QFile file(configName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(0, tr("config"),tr("Cannot write file %1:\n%2.").arg(configName).arg(file.errorString()));
        return;
    }

    if (rpconfig.writeConfig(&file))
    {
        QMessageBox::information(0,QObject::tr("XML config save"), QObject::tr("OK!"));
    }
    else
    {
        QMessageBox::critical(0,QObject::tr("XML config save erro"),QObject::tr("cannot save"));
    }
    file.close();
}
