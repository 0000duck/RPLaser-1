#include <QProgressDialog>
#include <QFont>
#include <QDir>
#include <QLibrary>
#include <QStringList>
#include <QMessageBox>
#include <QIcon>
#include "pluginManager.h"
#include "ui_pluginManager.h"
#include "mainwindow.h"

PluginManager::PluginManager(RPInterface *Iface) :
    QDialog(Iface),
    ui(new Ui::PluginManager),
    m_interface(Iface),
    b_listWidgetLock(false)
{
    ui->setupUi(this);
    setWindowTitle(tr("plugins"));

    //plugin catelogs.
       QListWidgetItem *listItem;
       QListWidgetItem *firstItem;
       int categoryType;
       QString categoryName;
       CategoryMap::iterator mIt;

       categoryType = CATAGORY_ALL;
       categoryName = QObject::tr("All Plugins");
       listItem = new QListWidgetItem(QIcon(QObject::tr(":/2D.png")),categoryName, ui->catalog,categoryType);
       m_categoryMap.insert(categoryType,listItem);
       firstItem = listItem;

       categoryType = CATAGORY_IMPORT;
       categoryName = QObject::tr("Import Format");
       listItem = new QListWidgetItem(QIcon(QObject::tr(":/2D.png")),categoryName, ui->catalog,categoryType);
       m_categoryMap.insert(categoryType,listItem);

       categoryType = CATAGORY_EXPORT;
       categoryName = QObject::tr("Export Format");
       listItem = new QListWidgetItem(QIcon(QObject::tr(":/3D.png")),categoryName, ui->catalog,categoryType);
       m_categoryMap.insert(categoryType,listItem);

       categoryType = CATAGORY_TEST;
       categoryName = QObject::tr("RP Machine Test");
       listItem = new QListWidgetItem(QIcon(QObject::tr(":/3D.png")),categoryName, ui->catalog,categoryType);
       m_categoryMap.insert(categoryType,listItem);

       categoryType = CATAGORY_DEBUG;
       categoryName = QObject::tr("RP algorithm Debug");
       listItem = new QListWidgetItem(QIcon(QObject::tr(":/3D.png")),categoryName, ui->catalog,categoryType);
       m_categoryMap.insert(categoryType,listItem);

       categoryType = CATAGORY_OTHER;
       categoryName = QObject::tr("others");
       listItem = new QListWidgetItem(QIcon(QObject::tr(":/3D.png")),categoryName, ui->catalog,categoryType);
       m_categoryMap.insert(categoryType,listItem);


       for(mIt = m_categoryMap.begin(); mIt!= m_categoryMap.end(); mIt++)
       {
            mIt.value()->setSizeHint(QSize(60,22));
       }

       QObject::connect(ui->catalog, SIGNAL(itemClicked(QListWidgetItem*)),
                       this, SLOT(chooseCat(QListWidgetItem*)) );

       QObject::connect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)),
                        this, SLOT(choosePlugin(QListWidgetItem*)) );

       QObject::connect(ui->listWidget, SIGNAL(itemSelectionChanged()),
                        this, SLOT(selectPlugin()) );

       searchPlugins();
       chooseCat(firstItem);
}

PluginManager::~PluginManager()
{
    b_listWidgetLock = true;
    clearListWidget();
    clearCateWidget();
    CategoryMap::iterator mIt;
    PluginItems::iterator pIt;
    for(mIt = m_categoryMap.begin(); mIt!= m_categoryMap.end(); mIt++)
    {
       delete mIt.value();
    }

    for(pIt = m_pluginItems.begin(); pIt!= m_pluginItems.end(); pIt++)
    {
       delete *pIt;
    }

    delete ui;
}

void PluginManager::clearListWidget()
{
    int count = ui->listWidget->count();
    for(int row = 0; row < count; row++)
    {
        ui->listWidget->takeItem(0);
    }
}

void PluginManager::clearCateWidget()
{
    int count = ui->catalog->count();
    for(int row = 0; row < count; row++)
    {
        ui->catalog->takeItem(0);
    }
}

void PluginManager::searchPlugins()
{
    QString sharedLibExtension,key;
    int catelog;
    QLibrary tlib;

#ifdef WIN32
sharedLibExtension = "*.dll";
#else
sharedLibExtension = "*.so*";
#endif

    QString myPluginDir = QDir::currentPath() + "/../plugins";
    QDir pluginDir( myPluginDir, sharedLibExtension, QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::NoSymLinks );

    if ( pluginDir.count() == 0 )
    {
       return;
    }
    for ( uint i = 0; i < pluginDir.count(); i++ )
    {
       QString libName = QString( "%1/%2" ).arg( myPluginDir ).arg( pluginDir[i] );
       tlib.setFileName(libName);

       if(tlib.load())
       {
            category_t *pcategory = (category_t*)( tlib.resolve( "category") );
            name_t *pName = ( name_t * ) ( tlib.resolve( "name" ) );
            description_t *pDesc = ( description_t * ) ( tlib.resolve( "description" ) );

            if(!pcategory || !pName)
            {
               tlib.unload();
               continue;
            }

            catelog = pcategory();
            if(!m_categoryMap.contains(catelog))
            {
                catelog = CATAGORY_OTHER;
            }

            QListWidgetItem* lstItem = new QListWidgetItem(pName(),0,catelog);
            lstItem->setData(Qt::AccessibleTextRole,libName);
            if(pDesc)
                lstItem->setData(Qt::AccessibleDescriptionRole,pDesc());
            key = getNamefrPath(libName);
            if(m_interface->m_plugins.contains(key))
            {
               lstItem->setCheckState(Qt::Checked);
            }
            else
            {
                lstItem->setCheckState(Qt::Unchecked);
            }
            lstItem->setSizeHint(QSize(60,22));
            m_pluginItems.push_back(lstItem);
            tlib.unload();
       }
    }
}

QString PluginManager::getNamefrPath(QString path)
{
    QStringList strList = path.split("/");
    unsigned knotsSize = strList.size();

    if(knotsSize > 0)
    {
        knotsSize--;
    }
    else
    {
         QMessageBox::critical(0,QObject::tr("plugin load"),QObject::tr("path is not correct"));
        return "NONE";
    }

    return strList.at(knotsSize);
}

void PluginManager::chooseCat(QListWidgetItem* sitem)
{
    ui->textEdit->setText("");
    int type = sitem->type();

    b_listWidgetLock = true;
    clearListWidget();
    PluginItems::iterator pIt;

    int index = 1;
    for(pIt = m_pluginItems.begin(); pIt != m_pluginItems.end(); pIt++ )
    {
        if(type !=CATAGORY_ALL &&(*pIt)->type() != type)
        {
            continue;
        }

        QString path = (*pIt)->data(Qt::AccessibleTextRole).toString();
        QString key = getNamefrPath(path);

        if(m_interface->m_plugins.contains(key))
        {
            (*pIt)->setCheckState(Qt::Checked);
        }

        ui->listWidget->insertItem(index,*pIt);
        index++;
    }
    b_listWidgetLock = false;

}

void PluginManager::selectPlugin()
{
    if(ui->listWidget->count() == 0 || b_listWidgetLock)
        return;

    QListWidgetItem *sitem = ui->listWidget->currentItem();

    if(sitem == NULL)
        return;

    //add to text editor.
    QString outputs = sitem->data(Qt::AccessibleDescriptionRole).toString();//AccessibleTextRole
    ui->textEdit->setText(outputs);
}

//load/unload plugin.
void PluginManager::choosePlugin(QListWidgetItem* sitem)
{
    b_listWidgetLock = true;

    //find is already on map.
    bool b_contain = false;

    //get item info.
    LoadedPlugins::iterator loadedIt;
    QString pluginName = sitem->text();
    QString path = sitem->data(Qt::AccessibleTextRole).toString();
    QString key = getNamefrPath(path);

    if(key =="NONE")
        return;

    if(m_interface->m_plugins.contains(key))
    {
        loadedIt = m_interface->m_plugins.find(key);
        b_contain = true;
    }

    if(sitem->checkState() == Qt::Checked)
    {
        if(!b_contain)//load
        {
            QLibrary *myLib = new QLibrary( path );
            RPPlugin *plugin = NULL;

            if ( !myLib->load() )
            {
               delete myLib;
               sitem->setCheckState(Qt::Unchecked);
               return;
            }

            plugin_t *creatPlugin = ( plugin_t * ) ( myLib->resolve( "classFactory" ) );
            if ( creatPlugin )
            {
              plugin = creatPlugin( m_interface );
            }

            LoadedPlugin loadedPlugin = LoadedPlugin(plugin,myLib);
            m_interface->m_plugins.insert(key,loadedPlugin);
            QMessageBox::information(0,QObject::tr("load plugin OK"),pluginName);
        }
        //esle b_contain do nothing.
    }
    else
    {
        if(b_contain)//unload
        {
            LoadedPlugin loadedPlugin =  m_interface->m_plugins[key];
            loadedPlugin.unload();
            m_interface->m_plugins.erase(loadedIt);
            QMessageBox::information(0,QObject::tr("unload plugin OK"),pluginName);
        }
        //else !b_cotain do nothing.

    }
    b_listWidgetLock = false;
}
