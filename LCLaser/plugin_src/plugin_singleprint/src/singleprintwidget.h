/***************************************************************************
                          XXXXXX.h -  description
                             -------------------
    begin                : Nov. 21 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#ifndef SMALLWIGET_H
#define SMALLWIGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QProgressDialog>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include "controlinterface.h"

namespace Ui{
class SinglePrintWidget;
}

class SinglePrintWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SinglePrintWidget(BuildRP::ControlInterface *icontrol,QWidget *parent = 0);
    ~SinglePrintWidget();
     void initPannel();

signals:

private slots:
    void on_comboBox_singelTrack_currentIndexChanged(int index);

    void on_spinBox_MaxLineHeight_valueChanged(int arg1);

    void on_doubleSpinBox_LengthRadius_valueChanged(double arg1);

    void on_doubleSpinBox_deltaZ_valueChanged(double arg1);

    void on_pushButton_trybuff_clicked();

    void on_pushButton_endbuff_clicked();

    void on_pushButton_RestoreBuff_clicked();

    void on_pushButton_clearbuff_clicked();

private:
    BuildRP::ControlInterface *rp_control;
    Ui::SinglePrintWidget *ui;
};

#endif // SMALLWIGET_H
