/***************************************************************************
                             data.h  -  description
                             -------------------
    begin                : Apr. 25 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef RPData_H
#define RPData_H
#include <QObject>
#include "dataSlice.h"
#include "dataDynamic.h"
#include "dataDrawTrack.h"
#include "triangleMesh.h"
#include "datainterface.h"
#include "measure.h"

namespace BuildRP
{
    class RPControl;
}

namespace SIMURP
{
    class GTMock;
}

class WTabGL;

class WTabData;

namespace DataRP
{

#define LARGE  1000

    typedef void extraOutline(BuildRP::RPControl* ,RPprintItem&, int &);
    typedef void extraPath(BuildRP::RPControl* ,Line&);

    class HandelScanPath;

    enum BarPosition
    {
        BAR_PROCESS = 0,
        BAR_PRINT,
    };

    enum SIMUSpeedType
    {
        SIMU_SAME = 0,
        SIMU_SLOW,
        SIMU_FAST,
    };

    class RPData : public DataInterface
    {
        Q_OBJECT

        friend class RPDrawMesh;
    public:
        RPData();
        ~RPData();

        //clear data
        void  clear();

        void clearSlices();

        void saveConfig();

        void clearMeasure();
        Measure *getMeasure();

        void animateShowLayers();

        void vertifyMeasurePos(int index);

        //!set and get.
        void setMaxLayer(double maxLayer);
        double getMaxLayer();

        void setMinLayer(double minLayer);
        double getMinLayer();

        void setMaxLevel(unsigned slevel);
        unsigned getMaxLevel();

        void setNumberOutlines(int noutlines);
        int  getNumberOutlines();

        void setCurrentLayer(int indexLayer);
        int  getCurrentLayer();

        void setCompenRadius(double radius);
        double getCompenRadius();

        void setPathInterval(double interval);
        double getPathInterval();

        void settoNumbLayer(int numbLayer);
        int gettoNumbLayer();

        void setFittingTrack(bool bs);
        bool isFittingTrack();

        double getLayerZ();
        void setLayerZ(double sz);

        void setBar(bool bshow, int postype);
        bool getBar(int postype);

        void setStopGenPath(bool bstop);
        bool isStopGenPath();

        void setCoordCompensation(bool bcomp);
        bool isCoordCompensation();

         //! set the starting cutPlane.
        void setCutPlanePoint();
        void resetCurPrintLayer();
        void setObjectToCenter();
        void setObjectToZeorZ();
        void terminateSimuTimer();


        void setDeltaZ(double deltaZ);

        //!get
        double getMinZ();
        double getMaxZ();

        TriangleMesh *getMesh();
        //! creat
        TriangleMesh *createNewMesh();
        //!slices
        void startSlice();

        //!cut plane
        Vec3D getCutPlanePeak();
        Vec3D getCutPlaneNormal();
        Vec3D getCutPlanePoint();

        //!outlines.
        void clearTheLayerOutlines(int layerIndex);
        void clearTheLayerPaths(int layerIndex);
        double getTheLayerZ(int layerIndex);
        unsigned getTheLayerOutlinesAmount(int layerIndex);
        RPboundaries& getTheLayerOuterBoundaries(int layerIndex);
        RPboundaries& getTheLayerlastBoundaries(int layerIndex);
        void addTheLayerBoundaries(int layerIndex,RPboundaries &newboundaries);
        void popbackTheLayerBoundaries(int layerIndex);

        void genPathsandOutlines(bool haspath, bool isAllLayers);
        void genPathsWithThread();

        //! judge the boundaries, if not empty, rendering.
        void checkStopPoint();

        bool slicesRenderable();

        bool meshRenderable();

        void setShowAll(bool bshow);
        void setShowMesh(bool bshow);
        void setShowCut(bool bshow);
        bool isShowAll();
        bool isShowMesh();
        bool isShowCut();
        bool isPrinting();

        //!transMat
        void setTransMat(Matrix4X4 stransMat);
        Matrix4X4 getTransMat();
        Matrix4X4 getSampleTransMat();
        void alignModel2Measure();
        void readTransMat();

       //!apply rotation.
        void applyRotateZ(double rz);
        void applyCutPlaneRot(double rz);
        static Vec3D rotate(Vec3D &RInput, Vec3D &ThisRot,bool clockwise = false);
        void setCutplaneRot(double srot);
        double getCutplaneRot();

        //! generate current layer's path.
        /*\param spathInterval the interveral of path.     */
        Vec3D getFirstPosition();
        void extractData(extraOutline *exline, extraPath *expath,BuildRP::RPControl* rpconntrol,RPprintItem &printItem,int &runtype, Line &line2);
        void setLaserHeadPosition(Vec3D pos);
        void clearTracks();
        Vec3D getLaserHeadPosition();

        void setAnimateTrack(Vec3D shead);
        void refreshAnimateTrack(Vec3D send);
        bool isTrackAnimate();
        void stopTrackAnimate();
        void getAnimateTrack(Vec3D &thead, Vec3D &tend);

        void setAnimateArc(Vec3D &start,Vec3D &center, double radius,double radian);
        void refreshAnimateArc(double radian);
        bool isArcAnimate();
        void stopArcAnimate();
        void getAnimateArc(Vec3D &start,Vec3D &center, double &radian, double &radius,double &trackWith);
        void getArc(Vec3D &start,Vec3D &center, double &radian, double &radius,double &trackWith);
        void getcurArcPos(Vec3D &pos);

        void refresh();

        void setLaserPriority(bool bp,unsigned short priority);
        void switchLaser(bool status, unsigned short priority);
        bool isLaserOn();

        bool isDrawProcess();
        void setDrawProcess(bool sdp);
        double getFrameInterval();
        void stepOn();
        void changeCommdsSize(unsigned sSize);
        void setCurrentCommdPos(unsigned spos);
        void clearLayerTracks();

        void exchangeDrawTraks();
        void clearDrawTrack();
        void reduceLayerTracks();
        void buildTracks(Line line);
        void buildArc(Vec3D send);
        //gtmock
        void setSimu(SIMURP::GTMock *sim);
        void setControl(BuildRP::RPControl* ctl);
        void setui(WTabData *tabdata);
        void setglTab(WTabGL *tabgl);

        BuildRP::RPControl* getControl();
        double getCurSimZ();
        int updateSimu();
        void increaseSimuLevel();
        void syntoPrinting();
        void resetTransMat();

        //core data
        HeadSlc m_SlcHeader;
        TableEntries m_SlcTableEntry;
        RPslices m_SlcSlices; //core data
        Vec3D m_Objcenter;
        BoundingBox m_boundingBox;
        Plane m_cutPlane;
        LayerTracks m_layerTracks;
        DrawTracks m_drawTracks;
        LastTrackItem m_lastTrackItem;
        SimuBallRoll *m_simuBall;
        float dataProcessPerc;
        int m_curPrintLevel;
        int m_curSimLevel;
        bool b_canLoad;

   signals:
        void updateDisplay();
        void printOK();
        void startListen();
        void endListen();
        void updateCommdsSize(unsigned);
        void updateCurrentCommdPos(unsigned);
        void activeDataTab();


    public slots:
        void simuPrintOK();

    protected:
        //! process
        HandelScanPath *m_scanPath;


    private:
        TriangleMesh *m_mesh;
        double m_maxLayerLevel;
        double m_minLayerLevel;
        unsigned m_maxLevel;
        int m_currentLayer;
        bool b_showall;
        bool b_showmesh;
        bool b_showcut;
        bool b_showProcess;

        bool bp_laserOn;
        bool b_coordCompensation;

        unsigned short priority_laserOn;
        volatile bool b_laserOn;

        Vec3D m_cutPlanePeak;
        double m_cutplaneRot;
        double m_compenRadius;
        double m_pathInterval;
        int m_numberOutlines;

        AnimateTrack m_animateTrack;
        AnimateArc   m_animateArc;
        Vec3D m_laserHeadPosition;

        double m_frameInterval;
        int m_toNumbLayer;
        bool b_fittingTrack;

        bool b_barProcess;
        bool b_barPrint;

        double m_rate;
        bool b_stopGenPath;
        double m_deltaZ;
        Matrix4X4 m_transMat;

        Measure *m_measure;

        SIMURP::GTMock *rp_sim;
        BuildRP::RPControl* rp_ctl;
        WTabData *ui_data;
        WTabGL *ui_gl;

    };
}


#endif // DATASLC_H
