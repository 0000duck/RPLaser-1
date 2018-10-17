/***************************************************************************
                          wizardbuttons.cpp  -  description
                             -------------------
    begin                : Jun. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QBitmap>
#include <QIcon>
#include <QToolBar>
#include <QMessageBox>
#include "mainwindow.h"
#include "wizardbuttons.h"
#include "control.h"

using namespace RenderRP;

WizardButtons::WizardButtons(QWidget *parent):
    QWidget(parent),
    m_wizardPreviousBtn(NULL),m_wizardNextBtn(NULL),m_dimensionBtn(NULL),
    m_pulseMoveBox(NULL)
{
    for(int i = 0; i < 6; i++)
    {
        m_directBtns[i] = NULL;
    }

    createBtns();
}

WizardButtons::~WizardButtons()
{
    if(m_wizardPreviousBtn != NULL)
        delete m_wizardPreviousBtn;

    if(m_wizardNextBtn != NULL)
        delete m_wizardNextBtn;

    if(m_dimensionBtn != NULL)
        delete m_dimensionBtn;

    if(m_pulseMoveBox != NULL)
        delete m_pulseMoveBox;

    for(int i = 0; i < 6; i++)
    {
        if(m_directBtns[i] != NULL)
            delete m_directBtns[i];
    }
}

void WizardButtons::setMainWin(MainWindow *mainw)
{
     m_mainw = mainw;

     if(m_mainw->rpDisplay->is2D())
     {
         m_dimensionBtn->setChecked(true);
     }
     else
         m_dimensionBtn->setChecked(false);

     setConnect();
     step0_start(1);
}


void WizardButtons::setGeometryPosition(QRect winSize)
{
    m_pulseMoveBox->setGeometry(0.5*winSize.width()-50, 0.5*winSize.height()+60,100,20);

    m_wizardPreviousBtn->setGeometry(0.5*winSize.width()-32, 0.90*winSize.height(),1,1);
    m_wizardNextBtn->setGeometry(0.5*winSize.width()+32, 0.90*winSize.height(),1,1);

    m_dimensionBtn->setGeometry(winSize.width()-40, 8,1,1);

    m_directBtns[0]-> setGeometry(0.5*winSize.width()-40, 0.5*winSize.height(),1,1);
    m_directBtns[1]-> setGeometry(0.5*winSize.width()+8, 0.5*winSize.height(),1,1);

    m_directBtns[2]-> setGeometry(0.5*winSize.width()-15, 0.5*winSize.height()-28,1,1);
    m_directBtns[3]-> setGeometry(0.5*winSize.width()-15, 0.5*winSize.height()+28,1,1);

    m_directBtns[4]-> setGeometry(0.5*winSize.width()+45, 0.5*winSize.height()-24,1,1);
    m_directBtns[5]-> setGeometry(0.5*winSize.width()+45, 0.5*winSize.height()+24,1,1);
}

void WizardButtons::createBtns()
{
    createWizardBtn();
    createDimensionBtn();
    createDirectionBtn();
    createPulseMoveBox();
}


void WizardButtons::setConnect()
{
    connect(m_wizardNextBtn,SIGNAL(clicked()),this,SLOT(on_wizardNext()));
    connect(m_wizardPreviousBtn,SIGNAL(clicked()),this,SLOT(on_wizardPrevious()));

    connect(m_dimensionBtn,SIGNAL(clicked(bool)),this,SLOT(on_dimensionBtn(bool)));

    connect( m_directBtns[0],SIGNAL(clicked()),this,SLOT(on_moveLeftX()));
    connect( m_directBtns[1],SIGNAL(clicked()),this,SLOT(on_moveRightX()));
    connect( m_directBtns[2],SIGNAL(clicked()),this,SLOT(on_moveForwardY()));
    connect( m_directBtns[3],SIGNAL(clicked()),this,SLOT(on_moveBackWardY()));
    connect( m_directBtns[4],SIGNAL(clicked()),this,SLOT(on_moveUpZ()));
    connect( m_directBtns[5],SIGNAL(clicked()),this,SLOT(on_moveDownZ()));
}


/*********************************create**************************************************/
void WizardButtons::createWizardBtn()
{
    m_wizardPreviousBtn = createBtn(":/preStep.png",QObject::tr("previus step"));
    m_wizardNextBtn = createBtn(":/nextStep.png",QObject::tr("next step"));
}

void WizardButtons::createDirectionBtn()
{
    m_directBtns[0] = createBtn(":/arrowLeft.png",QObject::tr("X left"));
    m_directBtns[1] = createBtn(":/arrowRight.png",QObject::tr("X right"));

    m_directBtns[2] = createBtn(":/arrowUp.png",QObject::tr("Y forward"));
    m_directBtns[3] = createBtn(":/arrowDown.png",QObject::tr("Y backward"));

    m_directBtns[4] = createBtn(":/arrowUp.png",QObject::tr("Z up"));
    m_directBtns[5] = createBtn(":/arrowDown.png",QObject::tr("Z down"));
}

void WizardButtons::createPulseMoveBox()
{
    m_pulseMoveBox = new QDoubleSpinBox(parentWidget());
    m_pulseMoveBox->setAutoFillBackground(true);
    m_pulseMoveBox->setBackgroundRole( QPalette::Base);
    m_pulseMoveBox->setRange(100,100000);
    m_pulseMoveBox->setValue(20000);
    m_pulseMoveBox->setSingleStep(2000);
}

void WizardButtons::createDimensionBtn()
{
    m_dimensionBtn = new QPushButton(parentWidget());

    m_dimensionBtn->clearMask();
    m_dimensionBtn->setCheckable(true);
    m_dimensionBtn->setBackgroundRole( QPalette::Base);

    QPixmap pixmap3D,pixmap2D;
    QIcon mIcon;
    pixmap3D.load(":/3Ds.png");
    pixmap2D.load(":/2Ds.png");
    QString descript = "dimension";
    mIcon.addPixmap(pixmap2D,QIcon::Normal,QIcon::On);
    mIcon.addPixmap(pixmap3D,QIcon::Normal,QIcon::Off);

    m_dimensionBtn->setStyleSheet("QPushButton{background-color:rgb(65,105,225)}");
    m_dimensionBtn->setFixedSize( 32,32 );
    m_dimensionBtn->setIcon(mIcon);
    m_dimensionBtn->setMask(pixmap3D.createHeuristicMask());
    m_dimensionBtn->setIconSize(QSize(32,32));
    m_dimensionBtn->setToolTip(descript);
    m_dimensionBtn->setCheckable(true);
}

QPushButton* WizardButtons::createBtn(QString imagePath,QString descript)
{
    QPushButton *pushButton= new QPushButton(parentWidget());

    pushButton->clearMask();
    pushButton->setBackgroundRole( QPalette::Base);

    QPixmap mypixmap;
    mypixmap.load(imagePath);

    //pushButton->setStyleSheet("QPushButton{background-color:rgb(255,0,0)}");
    pushButton->setFixedSize( mypixmap.width()+4, mypixmap.height()+4 );
    pushButton->setMask(mypixmap.createHeuristicMask());
    pushButton->setIcon(mypixmap);
    pushButton->setIconSize(QSize(mypixmap.width(),mypixmap.height()));
    pushButton->setToolTip(descript);
    return pushButton;
}

/***************************SLOTS***********************************/
void WizardButtons::on_wizardPrevious()
{
    if(m_currentStep > 0)
         m_currentStep--;

    stepManager(-1);
}

void WizardButtons::on_wizardNext()
{
    if(m_currentStep < 6)//7
         m_currentStep++;
    else
        m_currentStep = 0;

    stepManager(1);
}

void WizardButtons::on_dimensionBtn(bool checked)
{
    QPixmap pixmap3D,pixmap2D;
    pixmap3D.load(":/3Ds.png");
    pixmap2D.load(":/2Ds.png");
    m_dimensionBtn->clearMask();
    if(checked)
    {
        m_dimensionBtn->setMask(pixmap2D.createHeuristicMask());
        m_mainw->rpDisplay->setProjector(RenderRP::VORTHO);
    }
    else
    {
        m_dimensionBtn->setMask(pixmap3D.createHeuristicMask());
        m_mainw->rpDisplay->setProjector(RenderRP::VPERSPECTIVE);
    }
}

void WizardButtons::on_moveLeftX()
{
    double value = m_pulseMoveBox->value();
    double pos[3] = {0.0, 0.0, 0.0};
    if(RPConfig::content.getLeftX() == 1)
       pos[0] = value;
    else
       pos[0] = -value;

    m_mainw->rpControl->moveXYZ(pos[0],pos[1],pos[2]);
}

void WizardButtons::on_moveRightX()
{
    double value = m_pulseMoveBox->value();
    double pos[3] = {0.0, 0.0, 0.0};
    if(RPConfig::content.getLeftX() == 1)
       pos[0] = -value;
    else
       pos[0] = value;

    m_mainw->rpControl->moveXYZ(pos[0],pos[1],pos[2]);
}

void WizardButtons::on_moveForwardY()
{
    double value = m_pulseMoveBox->value();
    double pos[3] = {0.0, 0.0, 0.0};
    if(RPConfig::content.getForwardY() == 1)
       pos[1] = value;
    else
       pos[1] = -value;

    m_mainw->rpControl->moveXYZ(pos[0],pos[1],pos[2]);
}

void WizardButtons::on_moveBackWardY()
{
    double value = m_pulseMoveBox->value();
    double pos[3] = {0.0, 0.0, 0.0};
    if(RPConfig::content.getForwardY() == 1)
       pos[1] = -value;
    else
       pos[1] = value;

    m_mainw->rpControl->moveXYZ(pos[0],pos[1],pos[2]);
}

void WizardButtons::on_moveUpZ()
{
    double value = m_pulseMoveBox->value();
    double pos[3] = {0.0, 0.0, 0.0};
    if(RPConfig::content.getUpZ() == 1)
       pos[2] = value;
    else
       pos[2] = -value;

    m_mainw->rpControl->moveXYZ(pos[0],pos[1],pos[2]);
}

void WizardButtons::on_moveDownZ()
{
    double value = m_pulseMoveBox->value();
    double pos[3] = {0.0, 0.0, 0.0};
    if(RPConfig::content.getUpZ() == 1)
       pos[2] = -value;
    else
       pos[2] = value;

    m_mainw->rpControl->moveXYZ(pos[0],pos[1],pos[2]);
}


/*******************************hide show move*******************************************/
void WizardButtons::disableSteerXY()
{
    for(int i = 0; i < 4; i++)
    {
        m_directBtns[i]->setDisabled(true);
    }
}

void WizardButtons::setHideMoveSteer(bool bhide)
{
    for(int i = 0; i < 6; i++)
    {
       if(bhide)
           m_directBtns[i]->hide();
       else
           m_directBtns[i]->show();

    }

    if(bhide)
        m_pulseMoveBox->hide();
    else
        m_pulseMoveBox->show();


}

void WizardButtons::setHideWizardBtn(bool bhide, unsigned preNext)
{
         if(bhide)
         {
           if(preNext & 0x1)
               m_wizardPreviousBtn->hide();

           if(preNext & 0x2 )
               m_wizardNextBtn->hide();
         }
         else
         {
           if(preNext & 0x1)
             m_wizardPreviousBtn->show();

           if(preNext & 0x2 )
             m_wizardNextBtn->show();
         }
}

/**************************************step***************************************************/
unsigned WizardButtons::getCurrentStep()
{
    return m_currentStep;
}

void WizardButtons::stepManager(int direction)
{
    switch(m_currentStep)
    {
    case 0:
        step0_start(direction);
    break;

    case 1:
        step1_setPos(direction);
    break;

    case 2:
        step2_goHome(direction);
    break;

//    case 3:
//        step3_measure(direction);
//    break;

    case 3:
        step4_import(direction);
    break;

    case 4:
        step5_slice(direction);
    break;

    case 5:
        step6_genPath(direction);
    break;

    case 6:
        step7_print(direction);
    break;


    default:
    break;
    }

}

void WizardButtons::step0_start(int direction)
{
    if(direction == 1 || direction == -1)
    {
        //m_mainw->on_actionSimulator_triggered(true);
        m_mainw->rpData->clear();
        m_mainw->rpData->clearMeasure();
        m_mainw->rpDisplay->clearTracks();

        m_mainw->setTabEnable(1,false);
        m_mainw->setTabEnable(2,false);

        m_mainw->properDock->hide();
        m_mainw->toolBar->hide();
        m_mainw->tabGL->setShowLaserHead(false);
        setHideWizardBtn(true,1);
        setHideMoveSteer(true);
        m_currentStep = 0;
    }
     m_mainw->rpData->refresh();
}

void WizardButtons::step1_setPos(int direction)
{
    if(direction == 1 || direction == -1)
    {
        m_mainw->setCurrentTab(2);
        m_mainw->on_actionConnectMachine_triggered(true);
        setHideMoveSteer(false);
        setHideWizardBtn(false,3);
        m_mainw->tabGL->setShowLaserHead(true);      
    }
     m_mainw->rpData->refresh();
}

void WizardButtons::step2_goHome(int direction)
{
    if(direction == 1)
    {
        setHideMoveSteer(true);
        m_mainw->rpControl->goHome();
    }
     m_mainw->rpData->refresh();
}

void WizardButtons::step3_measure(int direction)
{
    m_mainw->setCurrentTab(0);
    if(direction == 1)
    {
        m_mainw->tabGL->readMeasureAndPos();
    }
     m_mainw->rpData->refresh();
}

void WizardButtons::step4_import(int direction)
{
    if(direction == 1)
    {
        m_mainw->on_actionImport_STL_triggered();
    }

    if(direction == -1)
    {
         m_mainw->rpData->refresh();
    }
}

void WizardButtons::step5_slice(int direction)
{
    if(direction == 1 )
    {
        m_mainw->tabGL->slice();
        m_mainw->rpData->setShowAll(true);
        m_mainw->tabData->setOutlineNumber(1);
    }

    m_mainw->rpData->refresh();

}

void WizardButtons::step6_genPath(int direction)
{
    if(direction == 1 )
    {
        m_mainw->rpData->setShowAll(false);
        setHideWizardBtn(false,3);
        m_mainw->toolBar->hide();
        m_mainw->tabData->setOutlineNumber(1);
        m_mainw->rpData->genPathsandOutlines(true,true);
        m_mainw->rpData->setBar(true,DataRP::BAR_PROCESS);
    }

         m_mainw->rpData->refresh();
}

void WizardButtons::step7_print(int direction)
{
     if(direction == 1 || direction == -1)
     {
         setHideWizardBtn(true,3);
         m_mainw->toolBar->show();
         m_mainw->on_actionPrint_triggered(true);
     }
}
