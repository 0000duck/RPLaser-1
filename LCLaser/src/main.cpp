/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Apr. 1st 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QElapsedTimer>
#include <QTextCodec>
#include <QTranslator>
#include <QDir>
#include "initialization.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    QTranslator translator(0);
    QString qmName;
    qmName = QDir::currentPath()+"/../share/ui.qm";
    translator.load(qmName,".");
    a.installTranslator(&translator);
    QSplashScreen splash(QPixmap(":/splash.png"));
    QElapsedTimer mtimer;
    mtimer.start();

    splash.show();
    splash.showMessage(QObject::tr("load configs ..."),Qt::AlignLeft|Qt::AlignBottom,Qt::green);
    while(mtimer.elapsed() <1000)
    {
        QCoreApplication::processEvents();
    }

    RPInitiate *init = new RPInitiate(&splash);
    init->getConfig();

    MainWindow w;
    w.show();
    splash.finish(&w);
    delete init;

    return a.exec();
}
