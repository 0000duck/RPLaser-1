/***************************************************************************
                         wizardbuttons.h  -  description
                             -------------------
    begin                : Jun. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef OVERLAY_BUTTON_H
#define OVERLAY_BUTTON_H

#include <QRect>
#include <QPushButton>
#include <QDoubleSpinBox>


class MainWindow;

namespace RenderRP
{
    class WizardButtons: public QWidget
    {
       Q_OBJECT
    public:
        explicit WizardButtons(QWidget *parent);
        ~WizardButtons();

        void createBtns();

        void setMainWin(MainWindow *mainw);
        void setGeometryPosition(QRect winSize);

        void disableSteerXY();
        void setHideMoveSteer(bool bhide);
        void setHideWizardBtn(bool bhide, unsigned preNext);

        unsigned getCurrentStep();

    protected:
        void createWizardBtn();
        void setConnect();

        void createDimensionBtn();
        void createDirectionBtn();
        void createPulseMoveBox();

        void stepManager(int direction);
        void step0_start(int direction);
        void step1_setPos(int direction);
        void step2_goHome(int direction);
        void step3_measure(int direction);
        void step4_import(int direction);
        void step5_slice(int direction);
        void step6_genPath(int direction);
        void step7_print(int direction);

   private slots:
        void on_wizardPrevious();
        void on_wizardNext();

        void on_dimensionBtn(bool checked);

        void on_moveLeftX();
        void on_moveRightX();
        void on_moveForwardY();
        void on_moveBackWardY();
        void on_moveUpZ();
        void on_moveDownZ();

    private:
         QPushButton* createBtn(QString imagePath,QString descript);

    private:
        QPushButton *m_wizardPreviousBtn;
        QPushButton *m_wizardNextBtn;
        QPushButton *m_dimensionBtn;

        QPushButton *m_directBtns[6];
        QDoubleSpinBox *m_pulseMoveBox;
        unsigned m_currentStep;

        MainWindow *m_mainw;

    };
}

#endif // OVERLAY_BUTTON_H
