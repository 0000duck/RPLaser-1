/***************************************************************************
                          mainwindow.h  -  description
                             -------------------
    begin                : Apr. 1st 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QTableWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPointer>
#include "data.h"
#include "control.h"
#include "glwin.h"
#include "tabgl.h"
#include "tabdata.h"
#include "display.h"
#include "tabcontrol.h"
#include "rpinterface.h"
#include "pluginManager.h"

namespace Ui {
class MainWindow;
class TabWidget;
}

class MainWindow : public RPInterface
{
    Q_OBJECT
public:
    friend class RenderRP::WizardButtons;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addPluginOnExportBar(RPPlugin *plugin);
    void addPluginOnModelTab(RPPlugin *plugin);
    void addPluginOnControlTab(RPPlugin *plugin);
    void showAlogrithm(bool sh);
    void setCurrentTab(int index);
    void setTabEnable(int index,bool benable);

protected:
    void setSubInterface();

   void createDockWidget();
   void addTabs();

   /************************************
   void timerEvent(QTimerEvent *event);
   QBasicTimer m_timer;
   int ct;
  *************************************/

private slots:
    void on_actionImport_STL_triggered();

    void on_actionImport_SLC_triggered();

    void on_actionImport_OBJ_triggered();

    void on_actionImport_PLY_triggered();

    void on_actionConnectMachine_triggered(bool checked);

    void on_actionLaserOn_triggered(bool checked);

    void on_actionSimulator_triggered(bool checked);

    void on_actionPrint_triggered(bool checked);

    void restorePrintButton();

    void on_actionHalt_triggered();

    void on_actionQuit_triggered();

    void on_actionSave_config_triggered();

    void on_activeDataTab();

    void on_actionExport_STL_triggered();

    void on_actionPluginManager_triggered();

private:
    Ui::MainWindow *ui;
    DataRP::RPData *rpData;
    RenderRP::RPDisplay *rpDisplay;
    BuildRP::RPControl *rpControl;
    QPointer<PluginManager> m_pluginManager;

    QDockWidget *properDock;
    QTabWidget *tabwidget;

    WTabGL *tabGL;
    WTabData *tabData;
    WTabControl *tabControl;
    QToolBar *toolBar;


};

#endif // MAINWINDOW_H
