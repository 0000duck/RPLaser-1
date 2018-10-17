/***************************************************************************
                          handelScanPath.h  -  description
                             -------------------
    begin                : Jul. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef HANDEL_OUTLINE_H
#define HANDEL_OUTLINE_H

#include <QObject>
#include "data.h"

namespace DataRP
{
    class HandelScanPath: public QObject
    {
    Q_OBJECT
    public:
        HandelScanPath(RPData *rpdata,QObject *parent = 0);
        ~HandelScanPath();

        void startGenScanPath();
        //!thread
        static void dealOuntlinesandPaths(HandelScanPath *dealpath);

        //all/current layers' outlines and paths.
        void genOutlinesandPaths(bool isAllLayers);

        //generate current layer's outlines.
        void genCurrentLayerOutlines();

        void setHasPath(bool has);

    public slots:
    void handleResults(QString str);

    protected:

        RPData *rp_data;
        int m_curLayer;
        double m_offset;
        int m_numboutlines;
        bool b_hasPath;

        //is fitting circle.
        void switchFittingTrack(bool bs);

        //gen outer and inner outlines.
        void genOutlines();

        //gen outer outer lines. subordinate of genOutlines.
        void genOuterOutlines();

        //gen outer inter lines. subordinate of genOutlines.
        void genInterOutlines(int number);

        void genCurLayerPath(double spathInterval, double scompensateRadius);

        void genPrintTrack(RPcontour &contour);

        void setCurlayerPathOK();

    private:

        static unsigned m_curCoursor;
        bool b_fittingTrack;

        //subordinate of genPrintTrack
        void  genArcPrintTrack(RPcontour &contour);
        void  genSimpleTrack(RPcontour &contour);

        //subordinate of genPrintTrack
        /* in factor judges two edges' clockwise direction be opposited.
        \param phead
        \param ptail
        \return  true if two points can be linked as a path       */
        bool canbePath(PathPoint &phead, PathPoint &ptail);
        void clearPath();

        virtual void genOutline(RPboundaries &inputboundaries,RPboundaries &outputboundaries)= 0;

    };
}

#endif // HANDEL_OUTLINE_H
