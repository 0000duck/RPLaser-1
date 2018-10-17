/***************************************************************************
                          rpplugin.h  -  description
                             -------------------
    begin                : Nov. 5 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef RPPLUGIN_IN
#define RPPLUGIN_IN

#include <QtCore/qglobal.h>
#include "rpinterface.h"
#include <QAction>
#include <QLayout>
#include <QWidget>

#ifdef WIN32
    #define RPEXPORT extern "C" __declspec(dllexport)
#else
    #define RPEXPORT extern "C"
#endif

enum CategoryType
{
    CATAGORY_ALL = 0,
    CATAGORY_OTHER,
    CATAGORY_IMPORT,
    CATAGORY_EXPORT,
    CATAGORY_CONFIG,
    CATAGORY_MADEUP,
    CATAGORY_TEST,
    CATAGORY_DEBUG,
};

class RPPlugin
{
public:

    RPPlugin()
        {}

    RPPlugin(RPInterface *itf):m_interface(itf)
        {}

        virtual ~RPPlugin()
        {}

    //! Get the name of the plugin
    static QString const & name()
    {
     return mName;
    }

    //! Version of the plugin
    static QString const & version()
    {
     return mVersion;
    }

    //! A brief description of the plugin
    static QString const & description()
    {
     return mDescription;
    }

    //! Plugin category
    static int const & category()
    {
     return mCategory;
    }


    static QString const &icon()
    {
        return mIcon;
    }

    QAction* getAction()
    {
        return m_action;
    }

    QLayout* getLayout()
    {
        return m_layout;
    }

    QWidget* getWidget()
    {
        return m_widget;
    }

    /// function to initialize connection to GUI
    virtual void initGui() = 0;

    //! Unload the plugin and cleanup the GUI
    virtual void unload() = 0;


protected:
    RPInterface *m_interface;

    QAction *m_action;

    QLayout *m_layout;

    QWidget *m_widget;

    void unloadAction()
    {
        if(m_action != NULL)
            delete m_action;
    }

    void unloadlayout()
    {
        if(m_layout != NULL)
            delete m_layout;
    }

    void unloadWidget()
    {
        if(m_widget != NULL)
            delete m_widget;
    }

private:
    /// plug-in name
    static const QString mName;

    /// description
    static const QString mDescription;

    /// category
    static const int mCategory;

    /// version
    static const QString mVersion;

    ///icon
    static const QString mIcon;

};
#endif // RPPLUGIN_IN
