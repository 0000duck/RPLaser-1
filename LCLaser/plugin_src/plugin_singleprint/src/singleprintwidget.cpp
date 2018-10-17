/***************************************************************************
                          XXXX.cpp  -  description
                             -------------------
    begin                : Nov. 21 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "singleprintwidget.h"
#include "ui_singleprintform.h"

SinglePrintWidget::SinglePrintWidget(BuildRP::ControlInterface *icontrol,QWidget *parent) :
    QWidget(parent),
    rp_control(icontrol),
    ui(new Ui::SinglePrintWidget)
{
    ui->setupUi(this);

    initPannel();
}

SinglePrintWidget::~SinglePrintWidget()
{
  delete ui;
}

void SinglePrintWidget::initPannel()
{
    ui->pushButton_RestoreBuff->setEnabled(false);
    ui->pushButton_endbuff->setEnabled(false);

    QString singleTrack = tr("LineZ,Line,Circle");
    QStringList itemList = singleTrack.split(",");
    ui->comboBox_singelTrack->addItems(itemList);

    rp_control->setSingleTrack(0);
    rp_control->setSingleHeight(ui->spinBox_MaxLineHeight->value());
    rp_control->setSingleLenghtRadius(ui->doubleSpinBox_LengthRadius->value());
    rp_control->SetSingleDeltaZ(ui->doubleSpinBox_deltaZ->value());
}

void SinglePrintWidget::on_comboBox_singelTrack_currentIndexChanged(int index)
{
    rp_control->setSingleTrack(index);
}

void SinglePrintWidget::on_spinBox_MaxLineHeight_valueChanged(int arg1)
{
    rp_control->setSingleHeight(arg1);
}

void SinglePrintWidget::on_doubleSpinBox_LengthRadius_valueChanged(double arg1)
{
    rp_control->setSingleLenghtRadius(arg1);
}

void SinglePrintWidget::on_doubleSpinBox_deltaZ_valueChanged(double arg1)
{
    rp_control->SetSingleDeltaZ(arg1);
}

void SinglePrintWidget::on_pushButton_trybuff_clicked()
{
    rp_control->singleBuffOn();
    ui->pushButton_trybuff->setEnabled(false);
    ui->pushButton_endbuff->setEnabled(true);
    ui->doubleSpinBox_LengthRadius->setEnabled(false);
    ui->doubleSpinBox_deltaZ->setEnabled(false);
    ui->comboBox_singelTrack->setEnabled(false);
    ui->spinBox_MaxLineHeight->setEnabled(false);
}

void SinglePrintWidget::on_pushButton_endbuff_clicked()
{
    rp_control->singleBuffOff();
    ui->pushButton_RestoreBuff->setEnabled(true);
    ui->pushButton_endbuff->setEnabled(false);
}

void SinglePrintWidget::on_pushButton_RestoreBuff_clicked()
{
    rp_control->singleBuffContinue();
    ui->pushButton_RestoreBuff->setEnabled(false);
    ui->pushButton_endbuff->setEnabled(true);
}

void SinglePrintWidget::on_pushButton_clearbuff_clicked()
{
    rp_control->SingleClearBuff();
    ui->pushButton_trybuff->setEnabled(true);
    ui->pushButton_RestoreBuff->setEnabled(false);
    ui->doubleSpinBox_LengthRadius->setEnabled(true);
    ui->doubleSpinBox_deltaZ->setEnabled(true);
    ui->comboBox_singelTrack->setEnabled(true);
    ui->spinBox_MaxLineHeight->setEnabled(true);
}
