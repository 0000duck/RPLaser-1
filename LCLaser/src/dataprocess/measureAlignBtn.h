/***************************************************************************
                          MeasureAlignBtn.h  -  description
                             -------------------
    begin                : Dec. 8 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#ifndef PARAMSET_H
#define PARAMSET_H

#include <QWidget>

class WTabGL;

namespace Ui {
class MeasureAlignBtn;
}

class MeasureAlignBtn : public QWidget
{
    Q_OBJECT
public:
    explicit MeasureAlignBtn(WTabGL *parent, int skey, int sindex = -1, int heightIndex = 0, bool blocBtn = false, int btnIndex = 0);
    ~MeasureAlignBtn();

    void setChecked(bool bc);

signals:

public slots:

private slots:

    void on_pushButton_set_clicked();

    void on_pushButton_delete_clicked();

protected:
    void normalSet();

    void localSet();

private:
    WTabGL *m_parent;
    Ui::MeasureAlignBtn *ui;
    int m_key;
    int m_index;
    int m_heightIndex;
    int m_genBtnCnt;
    bool b_locBtn;
    int m_btnIndex;
};

#endif // PARAMSET_H
