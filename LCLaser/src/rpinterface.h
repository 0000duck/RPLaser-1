/***************************************************************************
                          rpinterface.h  -  description
                             -------------------
    begin                : Nov. 5 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef RPINTERFACE_H_
#define RPINTERFACE_H_

#include <QMainWindow>
#include <QAction>
#include <QLibrary>
#include <QMap>
#include "controlinterface.h"
#include "datainterface.h"

class RPPlugin;

struct LoadedPlugin
{
    RPPlugin *m_plugin;
    QLibrary *m_lib;

    LoadedPlugin():
        m_plugin(NULL),m_lib(NULL)
    {}

    LoadedPlugin(RPPlugin *splugin,QLibrary *slib):
        m_plugin(splugin),m_lib(slib)
    {}

    void unload();
};

typedef QMap<QString, LoadedPlugin> LoadedPlugins;

class RPInterface : public QMainWindow
{
    Q_OBJECT

friend class PluginManager;

public:
    explicit RPInterface(QWidget *parent = 0);
    virtual ~RPInterface();

    virtual void addPluginOnExportBar(RPPlugin *plugin) = 0;

    virtual void addPluginOnModelTab(RPPlugin *plugin) = 0;

    virtual void addPluginOnControlTab(RPPlugin *plugin) = 0;

    virtual void showAlogrithm(bool sh) = 0;

   //virtual void addPluginOnPluginBar(QWidget *plugw) = 0;

protected:
    virtual void setSubInterface() = 0;

public:
    BuildRP::ControlInterface *iface_control;
    DataRP::DataInterface *iface_data;


private:
    LoadedPlugins m_plugins;
};

#endif // RPINTERFACE_H_
