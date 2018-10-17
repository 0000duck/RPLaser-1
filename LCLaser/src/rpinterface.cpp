#include "rpinterface.h"
#include "rpplugin.h"

RPInterface::RPInterface(QWidget *parent) :
    QMainWindow(parent)
{
}

RPInterface::~RPInterface()
{
}

void LoadedPlugin::unload()
{
    if(m_plugin)
    {
        m_plugin->unload();
        delete m_plugin;
        m_plugin = NULL;
    }

      if(m_lib)
    {
        m_lib->unload();
        delete m_lib;
        m_lib = NULL;
    }
}
