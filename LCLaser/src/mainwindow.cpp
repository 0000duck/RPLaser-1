/***************************************************************************
                          mainwindow.cpp  -  description
                             -------------------
    begin                : Apr. 1st 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QDebug>
#include <iostream>
#include <QFileDialog>
#include <QMenu>
#include <QWidget>
#include <QMessageBox>
#include <QScrollArea>
#include <QLayout>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ioslc.h"
#include "drawProcess.h"
#include "initialization.h"
#include "formatSTL.h"
#include "formatOBJ.h"
#include "formatPLY.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    RPInterface(parent),
    ui(new Ui::MainWindow),
    rpData(new DataRP::RPData),
    rpDisplay(new RenderRP::RPDisplay(rpData,this)),
    rpControl(new BuildRP::RPControl()),
    m_pluginManager(NULL),
    properDock(NULL),tabwidget(NULL)
{
    setSubInterface();
    //ui
    ui->setupUi(this);
    setCentralWidget(rpDisplay);
    createDockWidget();
    addTabs();

    setWindowTitle(QString::fromLocal8Bit("西安交通大学"));
    rpDisplay->setWizardButtons(this);

}

MainWindow::~MainWindow()
{
    delete ui;


    delete tabGL;
    delete tabData;
    delete tabControl;

    delete tabwidget;
    delete properDock;
    delete toolBar;

    delete rpControl;
    delete rpDisplay;
    delete rpData;
}

void MainWindow::setSubInterface()
{
     iface_control = rpControl;
     iface_data = rpData;
}


//creat side tool bar
void MainWindow::createDockWidget()
{
    properDock = new QDockWidget(tr("properties"),this);
    properDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    properDock->setWindowOpacity(0.5);

    tabwidget = new QTabWidget(properDock);
    tabwidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QSize block(266,512);
    tabwidget->setMinimumSize(block);

    properDock->setWidget(tabwidget);

    addDockWidget(Qt::RightDockWidgetArea,properDock);

    QMenu *viewMenu = ui->menuBar->findChild<QMenu *>("menuProperty");
    toolBar = ui->mainToolBar;
    toolBar->setWindowTitle(tr("tool bar"));

    if(viewMenu != NULL)
    {
        viewMenu->addAction(properDock->toggleViewAction());
        viewMenu->addAction(toolBar->toggleViewAction());
    }
}

//! add widget in tabs.
void MainWindow::addTabs()
{
    tabData = new WTabData(rpData);
    tabControl = new WTabControl(rpControl,rpData,this);
    tabGL = new WTabGL(rpDisplay);


    //opengl setting
    tabwidget->addTab(tabGL,tr("Measure"));
    connect(tabGL,SIGNAL(updateDisplay()),rpDisplay, SLOT(updateGL()));

    //data setting
    tabwidget->addTab(tabData,tr("Data"));
    connect(tabData,SIGNAL(activateTab(QWidget *)),tabwidget, SLOT(setCurrentWidget(QWidget *)));
    connect(tabData,SIGNAL(updateCommdsPos(unsigned)),rpDisplay,SLOT(gotoComdPos(unsigned)));

    connect(rpData,SIGNAL(printOK()),this,SLOT(restorePrintButton()));
    connect(rpData,SIGNAL(activeDataTab()),this,SLOT(on_activeDataTab()));

    connect(rpData,SIGNAL(updateCommdsSize(unsigned)), tabData, SLOT(shwCommdsSize(unsigned)));
    connect(rpData,SIGNAL(updateCurrentCommdPos(unsigned)), tabData, SLOT(shwCurrentCommdPos(unsigned)));

    connect(rpData,SIGNAL(updateDisplay()),rpDisplay,SLOT(updateGL()));
    connect(rpData,SIGNAL(startListen()),rpDisplay,SLOT(listern()));
    connect(rpData,SIGNAL(endListen()),rpDisplay,SLOT(listernOK()));
    tabwidget->setTabEnabled(1,false);

    //control setting
    tabwidget->addTab(tabControl,tr("Control"));//tabControl
    connect(rpControl,SIGNAL(showInfo(QString)),rpDisplay,SLOT(on_showInfo(QString)));
    connect(rpControl,SIGNAL(updateDisplay()),rpDisplay,SLOT(updateGL()));
    connect(rpControl, SIGNAL(updateLaserStatus()),tabControl, SLOT(getLaserStatus()));
    connect(tabControl,SIGNAL( showSteer(bool)),rpDisplay, SLOT(on_showSteer(bool)) );
    tabwidget->setTabEnabled(2,false);
    this->setWindowState(Qt::WindowMaximized);
}

void MainWindow::setTabEnable(int index,bool benable)
{
    if(tabwidget != NULL)
        tabwidget->setTabEnabled(index,benable);
}

void MainWindow::on_actionExport_STL_triggered()
{
    QString m_savepath = "../RP Documents";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save STL"), m_savepath,   "*.stl",  0);

    int index = fileName.lastIndexOf(".");
    fileName.truncate(index);
    fileName.append(".stl");

    if (fileName.isNull())
    {
        QMessageBox::critical(0,QObject::tr("cannot export STL file"),QObject::tr("file Name is empty"));
        return;
    }
    else
    {
        IORP::RPIOstl stl = IORP::RPIOstl(rpData);
        if(!stl.saveSTLfile(fileName,IORP::STL_ASCII))
        {
            QMessageBox::critical(0,QObject::tr("cannot export STL file"),fileName);
        }
    }
}

void MainWindow::on_actionImport_STL_triggered()
{
    QString m_openpath = "../RP Documents";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open STL"), m_openpath,   "*.stl",  0);

    if (fileName.isNull())
    {
        QMessageBox::critical(0,QObject::tr("cannot import STL file"),QObject::tr("file Name is empty"));
        return;
    }

    //clean old data.
    rpData->setStopGenPath(true);

    while(!rpData->b_canLoad)
    {
        #ifdef WIN32
                   Sleep(200);
        #else
                   usleep(200000);
        #endif
    }

    rpData->clear();
    rpData->setShowMesh(true);
    tabwidget->setTabEnabled(1,false);
    IORP::RPIOstl stl = IORP::RPIOstl(rpData);
    if(!stl.readSTLfile(fileName))
    {
        QMessageBox::critical(0,QObject::tr("cannot import STL file"),fileName);
    }
    rpData->alignModel2Measure();
    tabwidget->setCurrentIndex(0);
    rpData->setStopGenPath(false);
}

void MainWindow::setCurrentTab(int index)
{
    tabwidget->setCurrentIndex(index);
}

//! import slc file.
void MainWindow::on_actionImport_SLC_triggered()
{
    QString m_openpath = "../RP Documents";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open SLC"), m_openpath,   "*.slc",  0);

    if (fileName.isNull())
     {
       QMessageBox::critical(0,QObject::tr("cannot import SLC file"),QObject::tr("file Name is empty"));
       return;
     }

    IORP::RPIOslc slc;
    //clean old data.
    rpData->setStopGenPath(true);

    while(!rpData->b_canLoad)
    {
        #ifdef WIN32
                   Sleep(200);
        #else
                   usleep(200000);
        #endif
    }

    rpData->clear();
    slc.openFile(fileName,rpData);
    //update ui.
    tabData->slcImported();
    tabwidget->setTabEnabled(1,true);
    rpData->setStopGenPath(false);
}

//! import obj file.
void MainWindow::on_actionImport_OBJ_triggered()
{
    QString m_openpath = "../RP Documents";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open OBJ"), m_openpath,   "*.obj",  0);

    if (fileName.isNull())
    {
        QMessageBox::critical(0,QObject::tr("cannot import obj file"),QObject::tr("file Name is empty"));
        return;
    }

    //clean old data.
    rpData->setStopGenPath(true);

    while(!rpData->b_canLoad)
    {
        #ifdef WIN32
                   Sleep(200);
        #else
                   usleep(200000);
        #endif
    }

    rpData->clear();
    rpData->setShowMesh(true);
    tabwidget->setTabEnabled(1,false);
    IORP::RPIOobj obj = IORP::RPIOobj(rpData);
    if(!obj.readOBJfile(fileName))
    {
        QMessageBox::critical(0,QObject::tr("cannot import OBJ file"),fileName);
    }
    rpData->alignModel2Measure();
    tabwidget->setCurrentIndex(0);
    rpData->setStopGenPath(false);
}

//! import ply file.
void MainWindow::on_actionImport_PLY_triggered()
{
    QString m_openpath = "../RP Documents";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open PLY"), m_openpath,   "*.ply",  0);

    if (fileName.isNull())
    {
        QMessageBox::critical(0,QObject::tr("cannot import PLY file"),QObject::tr("file Name is empty"));
        return;
    }

    //clean old data.
    rpData->setStopGenPath(true);

    while(!rpData->b_canLoad)
    {
        #ifdef WIN32
                   Sleep(200);
        #else
                   usleep(200000);
        #endif
    }

    rpData->clear();
    rpData->setShowMesh(true);
    tabwidget->setTabEnabled(1,false);
    IORP::RPIOply ply = IORP::RPIOply(rpData);
    if(!ply.readPLYfile(fileName))
    {
        QMessageBox::critical(0,QObject::tr("cannot import PLY file"),fileName);
    }
    rpData->alignModel2Measure();
    tabwidget->setCurrentIndex(0);
    rpData->setStopGenPath(false);
}

void MainWindow::on_actionConnectMachine_triggered(bool checked)
{

    if(checked)
    {
        bool bconnec = rpControl -> connectMachine();

        if(bconnec)
        {
           //QMessageBox::information(0,QObject::tr("connect machine"), QObject::tr("connect OK"));
           tabwidget->setTabEnabled(2,true);
           ui->actionLaserOn->setEnabled(true);
           //tabwidget->setCurrentWidget(scrollControl);
           tabwidget->setCurrentWidget(tabControl);
           tabControl->setParam();
           tabControl->showStatus();
           tabControl->enableAxesEnable();
           ui->actionConnectMachine->setChecked(true);
        }
        else
        {
            ui->actionConnectMachine->setChecked(false);
            QMessageBox::critical(0,QObject::tr("connect machine"), QObject::tr("cannot connect"));
        }
    }
    else
    {
        //QMessageBox::information(0,QObject::tr("connect machine"), QObject::tr("disconnected machine"));
        rpControl -> turnOffLaser();
        rpControl ->disConnectMachine();
        ui->actionLaserOn->setChecked(false);
        ui->actionLaserOn->setEnabled(false);
        tabwidget->setTabEnabled(2,false);
        ui->actionConnectMachine->setChecked(false);
    }
//    QSize block(50,500);
//    tabwidget->setMinimumSize(block);
}

void MainWindow::on_actionLaserOn_triggered(bool checked)
{
     if(checked)
     {
         QMessageBox::information(0,QObject::tr("Laser"), QObject::tr("laser will turn on"));
         rpControl -> turnOnLaser();

     }
     else
     {
         //QMessageBox::information(0,QObject::tr("Laser"), QObject::tr("laser will turn off"));
         rpControl -> turnOffLaser();
     }
}

void MainWindow::on_actionSimulator_triggered(bool checked)
{
    if(rpControl != NULL)
    {
        rpControl->switchSIMU(checked);
        ui->actionSimulator->setChecked(checked);
    }
}

void MainWindow::on_actionPrint_triggered(bool checked)
{
    if(checked)
    {
        if(!rpControl->isPrintRun())
        {
            rpControl->startPrint(); //thread
            rpData->setBar(true,DataRP::BAR_PRINT);
        }
        else
        {
            rpControl->BuffContinue();
        }
        ui->actionPrint->setChecked(true);
    }
    else
    {
        if(rpControl!=NULL)
        {
            rpControl->BuffOff(); //thread
        }
        ui->actionPrint->setChecked(false);
    }
}

//! print OK and button recover start.
void MainWindow::restorePrintButton()
{
    ui->actionPrint->setChecked(false);
}

void MainWindow::on_actionHalt_triggered()
{
    if(rpControl==NULL)
      return;

    if(rpControl->isPrintRun())
    {
        rpControl->BuffOff(); //thread
        ui->actionPrint->setChecked(false);
    }
}

void MainWindow::on_actionQuit_triggered()
{
    rpControl->clearBuff();
    ui->actionPrint->setChecked(false);
    rpData->setBar(false,DataRP::BAR_PRINT);
    rpData->resetCurPrintLayer();
}

void MainWindow::on_actionSave_config_triggered()
{
    rpData->saveConfig();
    rpControl->saveConfig();
    RPInitiate init;
    init.recordConfig();
}

void MainWindow::on_activeDataTab()
{
    tabData->slcImported();
    tabwidget->setTabEnabled(1,true);
}

void MainWindow::on_actionPluginManager_triggered()
{
    if(m_pluginManager.isNull())
    {
        m_pluginManager = new PluginManager(this);
        m_pluginManager->setAttribute(Qt::WA_DeleteOnClose);
    }
        m_pluginManager->show();
}

void MainWindow::addPluginOnModelTab(RPPlugin *plugin)
{
    tabGL->attachWidget(plugin->getLayout());
}

void MainWindow::addPluginOnControlTab(RPPlugin *plugin)
{
    tabControl->attachWidget(plugin->getLayout());
}

void MainWindow::addPluginOnExportBar(RPPlugin *plugin)
{
    ui->menuExport->addAction(plugin->getAction());
}

void MainWindow::showAlogrithm(bool sh)
{
    tabData->showAlogrithm(sh);
}

