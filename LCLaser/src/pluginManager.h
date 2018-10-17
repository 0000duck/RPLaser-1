#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QListWidgetItem>
#include <QMap>
#include <QVector>
#include "rpinterface.h"
#include "rpplugin.h"


/* typedefs for plugins functions*/
typedef RPPlugin *plugin_t( RPInterface * iface );
typedef QString name_t();
typedef QString description_t();
typedef int category_t();

typedef QMap<int,QListWidgetItem*> CategoryMap;
typedef QVector<QListWidgetItem*> PluginItems;

namespace Ui {
class PluginManager;
}

class PluginManager : public QDialog
{
    Q_OBJECT

public:
    explicit PluginManager(RPInterface *Iface);
    ~PluginManager();

protected:
    void searchPlugins();
    void clearListWidget();
    void clearCateWidget();

private slots:
    void chooseCat(QListWidgetItem* sitem);
    void choosePlugin(QListWidgetItem* sitem);
    void selectPlugin();

private:
    QString getNamefrPath(QString path);

    CategoryMap m_categoryMap;
    PluginItems m_pluginItems;

    Ui::PluginManager *ui;
    RPInterface *m_interface;
    bool b_listWidgetLock;

};

#endif // PLUGIN_MANAGER_H
