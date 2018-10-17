/***************************************************************************
                          RPData.cpp  -  description
                             -------------------
    begin                : Apr. 25 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include <QDir>
#include <QMessageBox>
#include <QTimerEvent>
#include <algorithm>
#include "data.h"
#include "config.h"
#include "handelscanpath.h"
#include "handelBallOutline.h"
#include "gtmock.h"
#include "control.h"
#include "threadlog.h"
#include "tabdata.h"
#include "tabgl.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


using namespace DataRP;

Vec2D PathPoint::farpoint = Vec2D(0,0);


RPData::RPData():
    m_mesh(NULL),
    m_maxLayerLevel(0),
    m_maxLevel(0),
    m_currentLayer(0),
    b_showall(false),
    b_showmesh(false),
    b_showcut(false),
    b_showProcess(false),
    m_cutplaneRot(0.0),
    m_numberOutlines(0),
    m_frameInterval(10),
    m_toNumbLayer(0),
    b_barProcess(false),
    b_barPrint(false),
    m_rate(5.0),
    m_measure(new Measure())
{
    m_simuBall = new SimuBallRoll();
    m_scanPath = new HandelBallOutline(this);
    m_curPrintLevel = 0;
    m_curSimLevel = 0;
    b_canLoad = true;
    priority_laserOn = 10;
    bp_laserOn = false;
    //readTransMat();

    ui_gl = NULL;
}

RPData::~RPData()
{
    delete m_simuBall;

    if(m_mesh !=NULL)
    {
        delete m_mesh;
    }

     delete m_measure;
}

void RPData::clear()
{
    if(m_mesh != NULL)
    {
        delete m_mesh;
        m_mesh = NULL;
    }

    clearSlices();
}

 void RPData::clearSlices()
 {
 	m_SlcTableEntry.clear();
	m_SlcSlices.clear();
    m_maxLayerLevel = 0;
    m_maxLevel = 0;
    m_currentLayer = 0;
    m_Objcenter = Vec3D(0,0,0);
    m_boundingBox.clear();

    Vec3D snormal = Vec3D(0,1,0);
    m_cutPlane.setNormal(snormal);

    m_curPrintLevel = 0;
    m_curSimLevel = 0;
 }

void RPData::refresh()
{
    emit updateDisplay();
}

bool RPData::slicesRenderable()
{
    if(m_SlcSlices.size() > 0)
    {
        return true;
    }

    return false;
}

bool RPData::meshRenderable()
{
if(m_mesh == NULL)
    return false;

return true;
}

void RPData::clearMeasure()
{
    m_measure->clear();
}

Measure* RPData::getMeasure()
{
    return m_measure;
}

void RPData::simuPrintOK()
{
    refresh();
    emit printOK();
}

bool RPData::isShowAll()
{
    return b_showall;
}

bool RPData::isShowMesh()
{
    return b_showmesh;
}

bool RPData::isShowCut()
{
    return b_showcut;
}

void RPData::checkStopPoint()
{
    if(m_toNumbLayer == (int)m_maxLevel)
        return;

    if((m_toNumbLayer) == m_curPrintLevel)
    {
        rp_ctl->BuffOff();
        m_toNumbLayer = m_maxLevel;
        emit printOK();
    }
}

bool RPData::isPrinting()
{
    return rp_ctl->isPrintRun();
}

Vec3D RPData::getFirstPosition()
{
    if(m_SlcSlices.size() == 0 )
        return Vec3D(0.0, 0.0, 0.0);

    RPpolygon::iterator verIt;
    verIt = m_SlcSlices.begin()->outerBoundaries.begin()->vertList.begin();
    double minZ = m_SlcSlices.begin()->minZLevel;
    Vec2D firstPos2D = *verIt;
    Vec3D firstPos3D = firstPos2D.toVec3D(minZ);
    return firstPos3D;
}


/***************iterate all*****************/
void  RPData::extractData(extraOutline *exTrack, extraPath *expath,BuildRP::RPControl* rpconntrol,RPprintItem &printItem,int &runtype,Line &line2)
{
    RPslices::iterator itsli;
    RPboundaryGroup::iterator itbg;
    RPboundaries::iterator itbond;
    RPPrintTrack::iterator itTrack;
    RPlines2D::iterator itpath;
    Vec3D vecHead,vecTail;


    for(itsli = m_SlcSlices.begin(); itsli != m_SlcSlices.end(); itsli++ )
    {

        while(!itsli->isPathOK())
        {
#ifdef WIN32
            Sleep(800);
#else
            usleep(800000);
#endif
        }
//        //in case of no bounding.
//        if(itsli->outlines.size() < 1)
//            return;

        for(itbg = itsli->outlines.begin(); itbg !=  itsli->outlines.end(); itbg++)
        {
            for(itbond = itbg->begin(); itbond !=  itbg->end(); itbond++)
            {
                if(itbond->vertList.size()< 3)
                {
                    qDebug()<<"vertList is too short"<<endl;
                    return;
                }

                //move to start position.
                runtype = TRAVEL_EMPTY;
                itTrack = itbond->printTrack.begin();
                itTrack->setZ(itsli->minZLevel);
                printItem = *itTrack;
                exTrack(rpconntrol,printItem,runtype);//call back

                for(itTrack = itbond->printTrack.begin()+1; itTrack != itbond->printTrack.end(); itTrack++)
                {
                    runtype = TRAVEL_PRINT;
                    itTrack->setZ(itsli->minZLevel);
                    printItem = *itTrack;
                    exTrack(rpconntrol,printItem,runtype);//call back
                }//vertex

                runtype = TRAVEL_LASEROFF;
                exTrack(rpconntrol,printItem,runtype);//call back
             }//boundaries
        }//outlines

        for(itpath = itsli->paths.begin(); itpath !=  itsli->paths.end(); itpath++)
        {
            vecHead = Vec3D(itpath->getHead().getX(),itpath->getHead().getY(),itsli->minZLevel);
            vecTail = Vec3D(itpath->getTail().getX(),itpath->getTail().getY(),itsli->minZLevel);
            line2 = Line(vecHead,vecTail);
            expath(rpconntrol,line2);//call back
        }
        //end a layer.
        runtype = TRAVEL_LAYEROVER;
        printItem = RPprintItem();
        exTrack(rpconntrol,printItem,runtype);
     }//slices

     //send finish command, and set the laeser head to the maxlevel.
     runtype = TRAVEL_FINISH;
     vecTail = Vec3D(0,0,m_maxLayerLevel);
     printItem = RPprintItem(vecTail);
     exTrack(rpconntrol,printItem,runtype);
}

/***************rotation********************/
void RPData::applyRotateZ(double rz)
{
    DataRP::RPslices::iterator itsli;
    DataRP::RPboundaries::iterator itbond;
    DataRP::RPpolygon::iterator itver;
    Vec3D vecResult;
    Vec3D Rotby = Vec3D(0,0,rz);

    for(itsli = m_SlcSlices.begin(); itsli != m_SlcSlices.end(); itsli++ )
    {
        for(itbond = itsli->outerBoundaries.begin(); itbond !=  itsli->outerBoundaries.end(); itbond++)
        {
            for(itver = itbond->vertList.begin(); itver != itbond->vertList.end(); itver++)
            {
                vecResult = Vec3D(itver->x,itver->y,itsli->minZLevel);
                rotate(vecResult,Rotby);
                itver->x = vecResult.getX();
                itver->y = vecResult.getY();
            }//vertex
         }//outerBoundaries
       }//slices

    //rotate boundingBox CALLBACK.
    m_boundingBox.SrotateZ(rotate,Rotby,true);//vecResult,
    setCutPlanePoint();

    emit updateDisplay();
}

Vec3D RPData::rotate(Vec3D &RInput, Vec3D &RBy, bool clockwise)
{
    double wise;
    if (clockwise)
        wise = -1.0;
    else
        wise = 1.0;

   RInput.RotZ(DEG2RAD(RBy.z)*wise);
   return RInput;
}

void RPData::applyCutPlaneRot(double rz)
{
    //set normal
    Vec3D planeNormal  = m_cutPlane.getNormal();
    Vec3D Rotby = Vec3D(0,0,rz);
    rotate(planeNormal,Rotby,true);
    m_cutPlane.setNormal(planeNormal);
    //set point
    setCutPlanePoint();
    emit updateDisplay();
}

/*************get and set *******************/

void RPData::saveConfig()
{
    double rotz,transx,transy;

    //ui_gl->getCompensationParam(rotz,transx,transy);

    RPConfig::content.setCoordcompensation(isCoordCompensation());

    RPConfig::content.setRotZ(rotz);
    RPConfig::content.setTransX(transx);
    RPConfig::content.setTransY(transy);

    RPConfig::content.setLightSpotRadius(m_compenRadius);
    RPConfig::content.setPathInterval(m_pathInterval);
    RPConfig::content.setHeightEachLayer(m_deltaZ);
}

void RPData::setui(WTabData *tabdata)
{
    ui_data = tabdata;
}

void RPData::setglTab(WTabGL *tabgl)
{
    ui_gl = tabgl;
}

void RPData::setCutplaneRot(double srot)
{
    m_cutplaneRot = srot;
}

double RPData::getCutplaneRot()
{
    return m_cutplaneRot;
}

void RPData::setCompenRadius(double radius)
{
    m_compenRadius = radius;
}

void RPData::setPathInterval(double interval)
{
    m_pathInterval = interval;
}

double RPData::getPathInterval()
{
    return m_pathInterval;
}

double RPData::getCompenRadius()
{
    return m_compenRadius;
}

void RPData::setNumberOutlines(int noutlines)
{
    m_numberOutlines = noutlines;
}

int RPData::getNumberOutlines()
{
    return m_numberOutlines;
}

double RPData::getMaxLayer()
{
    return m_maxLayerLevel;
}

void RPData::setMaxLayer(double maxLayer)
{
    m_maxLayerLevel = maxLayer;
    m_boundingBox.MinMax[2][1] = m_maxLayerLevel;
}

void RPData::setMinLayer(double minLayer)
{
    m_minLayerLevel = minLayer;
    m_boundingBox.MinMax[2][0] = m_minLayerLevel;
}

double RPData::getMinLayer()
{
    return m_minLayerLevel;
}

unsigned RPData::getMaxLevel()
{
    return m_maxLevel;
}

void RPData::setMaxLevel(unsigned slevel)
{
    m_maxLevel = slevel;
}

void RPData::setCurrentLayer(int indexLayer)
{
    m_currentLayer = indexLayer;
    emit updateDisplay();

}

int RPData::getCurrentLayer()
{
    return m_currentLayer;
}

void RPData::setShowAll(bool bshow)
{
    b_showall = bshow;
    emit updateDisplay();
}

void RPData::setShowMesh(bool bshow)
{
    b_showmesh = bshow;
    emit updateDisplay();
}

void RPData::setShowCut(bool bshow)
{
    b_showcut = bshow;
    emit updateDisplay();
}

void RPData::setObjectToZeorZ()
{
    double dz = m_boundingBox.getminZ();
    RPslices::iterator itsli;
    RPboundaries::iterator itbond;
    for(itsli = m_SlcSlices.begin(); itsli != m_SlcSlices.end(); itsli++ )
    {
        itsli->minZLevel -= dz;
        for(itbond = itsli->outerBoundaries.begin(); itbond !=  itsli->outerBoundaries.end(); itbond++)
        {
            itbond->calnumVerts();
        }//outerBoundaries
    }

    m_boundingBox.MinMax[2][0] -= dz;
    m_boundingBox.MinMax[2][1] -= dz;
    setMinLayer(m_boundingBox.getminZ());
    setMaxLayer(m_boundingBox.getmaxZ());
    ui_data->resetInfo();
}

void RPData::setObjectToCenter()
{
    Vec3D center = m_SlcHeader.extents.getCenter();
    double minZ = m_SlcHeader.extents.minz;

    if(center == m_Objcenter && minZ == 0 )
    {
        return;
    }

    m_Objcenter = center;
    double dx = m_Objcenter.getX();
    double dy = m_Objcenter.getY();
    double dz = minZ;

    RPslices::iterator itsli;
    RPboundaries::iterator itbond;
    RPpolygon::iterator itver;

    for(itsli = m_SlcSlices.begin(); itsli != m_SlcSlices.end(); itsli++ )
    {
        //in case of actual minimal z less than the labelled.
        if(itsli == m_SlcSlices.begin())
        {
            if(itsli->minZLevel < dz)
                dz = itsli->minZLevel;
        }

        itsli->minZLevel -= dz;
        for(itbond = itsli->outerBoundaries.begin(); itbond !=  itsli->outerBoundaries.end(); itbond++)
        {
            for(itver = itbond->vertList.begin(); itver != itbond->vertList.end(); itver++)
            {
              itver->x -= dx;
              itver->y -= dy;
            }//vertex
            itbond->calnumVerts();
         }//outerBoundaries
       }//slices

       m_boundingBox.setBounding(m_SlcHeader.extents.minx -dx,m_SlcHeader.extents.maxx -dx,
                                 m_SlcHeader.extents.miny -dy,m_SlcHeader.extents.maxy -dy,
                                 m_SlcHeader.extents.minz -dz,m_SlcHeader.extents.maxz -dz);

       setCutPlanePoint();
       emit updateDisplay();
}

void RPData::setCutPlanePoint()
{
    Vec3D  planePoint;
    double minX,maxX,minY,maxY;
    Vec3D  axeXYZ[4];
    double dotValue[4];

    axeXYZ[0] = Vec3D(1,0,0);
    axeXYZ[1] = Vec3D(0,1,0);
    axeXYZ[2] = Vec3D(-1,0,0);
    axeXYZ[3] = Vec3D(0,-1,0);

    Vec3D  normal = m_cutPlane.getNormal();
    double lz = m_SlcSlices.at((unsigned)m_currentLayer).getZ();

    for(int i = 0; i < 4; i++)
    {
    dotValue[i] = normal.Dot(axeXYZ[i]);
    }

    m_boundingBox.getMaxDownEdgeXY(minX,maxX,minY,maxY);

    if (dotValue[0] > 0 && dotValue[1] >= 0)
    {
        planePoint = Vec3D(minX,minY,lz);
        m_cutPlanePeak = Vec3D(maxX,maxY,lz);
    }
    else if (dotValue[1] > 0 && dotValue[2] >= 0)
    {
        planePoint = Vec3D(maxX,minY,lz);
        m_cutPlanePeak = Vec3D(minX,maxY,lz);
    }
    else if (dotValue[2] > 0 && dotValue[3] >= 0)
    {
        planePoint = Vec3D(maxX,maxY,lz);
        m_cutPlanePeak = Vec3D(minX,minY,lz);
    }
    else
    {
        planePoint = Vec3D(minX,maxY,lz);
        m_cutPlanePeak = Vec3D(maxX,minY,lz);
    }

    m_cutPlane.reset(normal,planePoint);
}

void RPData::terminateSimuTimer()
{
    rp_sim->stopTimer();
}

void RPData::setStopGenPath(bool bstop)
{
     b_stopGenPath = bstop;
}

 bool RPData::isStopGenPath()
 {
     return b_stopGenPath;
 }

Vec3D RPData::getCutPlanePeak()
{
    return m_cutPlanePeak;
}

Vec3D RPData::getCutPlaneNormal()
{
    return m_cutPlane.getNormal();
}

Vec3D RPData::getCutPlanePoint()
{
    return m_cutPlane.getPoint(); //z coordinate of the slice.
}


double RPData::getMinZ()
{
    Vec3D pos;
    pos = m_boundingBox.getDownBotLeft();
    return pos.getZ();
}

double RPData::getMaxZ()
{
    Vec3D pos;
    pos = m_boundingBox.getUpTopRight();
    return pos.getZ();
}

double RPData::getFrameInterval()
{
    return m_frameInterval;
}

void RPData::settoNumbLayer(int numbLayer)
{
    m_toNumbLayer = numbLayer;
}

int RPData::gettoNumbLayer()
{
    return m_toNumbLayer;
}

double  RPData::getLayerZ()
{
    DataRP::RPslices::iterator itslic;
    double startZ,endZ;
    itslic = m_SlcSlices.begin();
    startZ = itslic->minZLevel;

    itslic = m_SlcSlices.begin()+1;
    endZ =  itslic->minZLevel;

    return endZ - startZ;
}

void  RPData::setLayerZ(double sz)
{
    DataRP::RPslices::iterator itslic;
    double startZ;
    itslic = m_SlcSlices.begin();
    startZ = itslic->minZLevel;

    unsigned nlayer = 0;
    for(itslic = m_SlcSlices.begin(); itslic != m_SlcSlices.end(); itslic++ )
    {
          itslic->minZLevel = startZ + nlayer*sz;
          nlayer++;
    }//slices
    //qDebug()<<"setLayerZ"<<endl;
}

void RPData::setBar(bool bshow, int postype)
{
    if(postype == BAR_PROCESS)
    {
       b_barProcess = bshow;
    }
    else if(postype == BAR_PRINT)
    {
       b_barPrint = bshow;
    }
}

bool RPData::getBar(int postype)
{
    if(postype == BAR_PROCESS)
    {
       return b_barProcess;
    }

    if(postype == BAR_PRINT)
    {
       return b_barPrint;
    }

    return false;
}

TriangleMesh* RPData::getMesh()
{
    return m_mesh;
}

void RPData::resetCurPrintLayer()
{
    m_curPrintLevel = 0;
    m_curSimLevel = 0;
}

/******************create*************************/

TriangleMesh* RPData::createNewMesh()
{
    if(m_mesh !=NULL)
    {
        delete m_mesh;
        m_mesh = NULL;
    }

    m_mesh = new TriangleMesh();
    return m_mesh;

}

/******************do slice*************************/
 void RPData::startSlice()
 {
    if(m_mesh == NULL)
        return;

    clearSlices();

    double judgeDot = 0.999999;
    VerticesGrp boundaries;
    RPpolygon vertices;
    RPpolygon::iterator tempIt;

    m_boundingBox = m_mesh->m_bbox;
    double minZ = m_boundingBox.getminZ();
    double maxZ = m_boundingBox.getmaxZ();
    double stepZ = m_deltaZ;
    double curZ = minZ;
    short traveType = DataRP::TRAVE_EVEN;

    Vec3D planeNormal = Vec3D(0.0, 0.0, 1.0);
    Vec3D planePos = Vec3D(0.0, 0.0, curZ);
    Plane plane = Plane(planeNormal,planePos);
    m_mesh->setFacesTraveType(traveType);

    Vec2D newEdgeNormal,oldEdgeNormal,v1,v2;

    VerticesGrp::iterator boundaryIt;
    RPVertices::iterator vertIt;

    while(curZ < maxZ)
    {
        if(traveType ==  DataRP::TRAVE_EVEN)
            traveType = DataRP::TRAVE_ODD;
        else if(traveType ==  DataRP::TRAVE_ODD)
            traveType = DataRP::TRAVE_EVEN;

        planePos = Vec3D(0.0, 0.0, curZ);
        plane.setPoint(planePos);
        DataRP::RPcontour contour;
        DataRP::RPslice slice;

        m_mesh->cutByPlane(plane, boundaries, traveType);

        for(boundaryIt = boundaries.begin(); boundaryIt != boundaries.end();boundaryIt++)
        {
            for(vertIt = boundaryIt->begin();vertIt != boundaryIt->end(); vertIt++)
            {
                Vec2D point(vertIt->getX(),vertIt->getY());

                if(vertices.size() > 1)
                {
                    v1 = *(vertices.end()-2);
                    v2 = *(vertices.end()-1);
                    oldEdgeNormal = v2 - v1;
                    oldEdgeNormal.Normalize();

                    newEdgeNormal = point - v2;
                    newEdgeNormal.Normalize();

                    if(newEdgeNormal.Dot(oldEdgeNormal) > judgeDot)
                    //if(newEdgeNormal.IsApproximateEqual(oldEdgeNormal,3.0))
                    {
                        vertices.pop_back();
                    }
                }
                vertices.push_back(point);
            }

            if(vertices.size() < 4)
            {
               QMessageBox::critical(0,QObject::tr("boundary too few"), QObject::tr("boundary vertices are too few"));
               continue;
            }

            v1 = *(vertices.end()-2);
            v2 = *(vertices.end()-1);
            oldEdgeNormal = v2 - v1;
            oldEdgeNormal.Normalize();

            v1 = *(vertices.begin());
            v2 = *(vertices.begin()+1);
            newEdgeNormal = v2 - v1;
            newEdgeNormal.Normalize();

            if(newEdgeNormal.Dot(oldEdgeNormal) > judgeDot)//merge save direction lines as one.
            //if(newEdgeNormal.IsApproximateEqual(oldEdgeNormal,3.0))
            {
                vertices.pop_back();
                for(tempIt = vertices.begin()+1; tempIt != vertices.end(); tempIt++)
                {
                    contour.vertList.push_back(*tempIt);
                }
                 contour.vertList.push_back(*(vertices.begin()+1));
            }
            else
            {
                 std::copy(vertices.begin(),vertices.end(), std::back_inserter(contour.vertList));
            }
            //reverse(contour.vertList.begin(),contour.vertList.end());
            vertices.clear();
            contour.calnumVerts();
            slice.outerBoundaries.push_back(contour);
            contour.clear();
        }
        slice.minZLevel = curZ;
        slice.numb_outerBoundaries = slice.outerBoundaries.size();

        if(slice.numb_outerBoundaries > 0)
            m_SlcSlices.push_back(slice);

        slice.clear();
        boundaries.clear();
        curZ += stepZ;
    }

    setMaxLayer(maxZ);
    setMinLayer(minZ);
    setMaxLevel(m_SlcSlices.size());
    setShowMesh(false);
    emit activeDataTab();
 }

 void RPData::setDeltaZ(double deltaZ)
 {
     m_deltaZ = deltaZ;
 }

/******************Animation*************************/
void RPData::setAnimateTrack(Vec3D shead)
{
    m_animateTrack.setActive(shead);
}

void RPData::refreshAnimateTrack(Vec3D send)
{
    m_animateTrack.refreshTrack(send);
}

bool RPData::isTrackAnimate()
{
    return m_animateTrack.isActive();
}

void  RPData::stopTrackAnimate()
{
    m_animateTrack.disActive();
}

void RPData::getAnimateTrack(Vec3D &thead, Vec3D &tend)
{
    thead = m_animateTrack.getHead();
    tend = m_animateTrack.getTail();
}

void RPData::setAnimateArc(Vec3D &start,Vec3D &center, double radius,double radian)
{
    m_animateArc.setActive(start,center, radius,radian);
}

void RPData::refreshAnimateArc(double radian)
{
    m_animateArc.refreshArc(radian);
}

bool RPData::isArcAnimate()
{
    return m_animateArc.isActive();
}

void RPData::stopArcAnimate()
{
    m_animateArc.disActive();
}

void RPData::getAnimateArc(Vec3D &start,Vec3D &center, double &radian, double &radius,double &trackWith)
{
     m_animateArc.getAnimValue(start,center,radian,radius);
     trackWith = getCompenRadius();
}

void RPData::getArc(Vec3D &start,Vec3D &center, double &radian, double &radius,double &trackWith)
{
     m_animateArc.getValue(start,center,radian,radius);
     trackWith = getCompenRadius();
}

void RPData::getcurArcPos(Vec3D &pos)
{
     m_animateArc.getcurrPos(pos);
}

void RPData::clearTracks()
{
     clearLayerTracks();
}

/******************Laser*************************/
void RPData::setLaserHeadPosition(Vec3D pos)
{
    m_laserHeadPosition = pos;
}

Vec3D RPData::getLaserHeadPosition()
{
    return m_laserHeadPosition;
}

void RPData::setLaserPriority(bool bp, unsigned short priority)
{
    bp_laserOn = bp;
    priority_laserOn = priority;

}

void RPData::switchLaser(bool status, unsigned short priority)
{
    if(bp_laserOn)
    {

        if(priority > priority_laserOn)
        {
            return;
        }

        b_laserOn = status;
    }
    else
    {
         b_laserOn = status;
    }
}

bool RPData::isLaserOn()
{
    return b_laserOn;
}

/******************genarate outline and path*************************/
void RPData::genPathsWithThread()
{
    m_scanPath->startGenScanPath();
    rp_sim->listernCommends();
}

double RPData::getTheLayerZ(int layerIndex)
{
    return m_SlcSlices.at((unsigned)layerIndex).getZ();
}

unsigned RPData::getTheLayerOutlinesAmount(int layerIndex)
{
    return m_SlcSlices.at(layerIndex).outlines.size();
}

void RPData::clearTheLayerOutlines(int layerIndex)
{
    m_SlcSlices.at(layerIndex).outlines.clear();
}

void RPData::clearTheLayerPaths(int layerIndex)
{
    m_SlcSlices.at(layerIndex).paths.clear();
}

RPboundaries& RPData::getTheLayerOuterBoundaries(int layerIndex)
{
    return m_SlcSlices.at(layerIndex).outerBoundaries;
}

RPboundaries& RPData::getTheLayerlastBoundaries(int layerIndex)
{
    return *(m_SlcSlices.at(layerIndex).outlines.end()-1);
}

void RPData::addTheLayerBoundaries(int layerIndex,RPboundaries &newboundaries)
{
    m_SlcSlices.at(layerIndex).outlines.push_back(newboundaries);
}

void RPData::popbackTheLayerBoundaries(int layerIndex)
{
    m_SlcSlices.at(layerIndex).outlines.pop_back();
}

void RPData::genPathsandOutlines(bool haspath, bool isAllLayers)
{
   if(m_SlcSlices.size()== 0)
       return;

    m_scanPath->setHasPath(haspath);
    if(!isAllLayers)
    {
        m_scanPath->genCurrentLayerOutlines();
    }
    else
    {
        genPathsWithThread();
    }
}

/******************Simulation***************************/
bool RPData::isDrawProcess()
{
   return b_showProcess;
}

void RPData::setDrawProcess(bool sdp)
{
   b_showProcess = sdp;
   m_simuBall->setActive(b_showProcess);

   if(b_showProcess)
   {
         emit startListen();
   }
   else
   {
         emit endListen();
   }

   emit updateDisplay();
}

void RPData::setCurrentCommdPos(unsigned spos)
{
    emit updateCurrentCommdPos(spos);
}

void RPData::stepOn()
{
   m_simuBall->goOn(true);
}

void RPData::changeCommdsSize(unsigned sSize)
{
    emit updateCommdsSize(sSize);
}

/******************draw track***************************/
void RPData::exchangeDrawTraks()
{
    m_layerTracks.push_back(m_drawTracks);
    clearDrawTrack();
    reduceLayerTracks();
}

void RPData::clearDrawTrack()
{
    m_drawTracks.clear();
}

void RPData::clearLayerTracks()
{
    LayerTracks::iterator lit;
    DrawTracks::iterator pt_it;
    for(lit = m_layerTracks.begin(); lit != m_layerTracks.end(); lit++)
    {
        for(pt_it = lit->begin(); pt_it != lit->end(); pt_it++)
        {
            if (*pt_it != NULL)
            {
                delete *pt_it;
            }
            *pt_it =NULL;
        }
        lit->clear();
    }
    m_layerTracks.clear();

    for(pt_it = m_drawTracks.begin(); pt_it != m_drawTracks.end(); pt_it++)
    {
        if (*pt_it != NULL)
        {
            delete *pt_it;
        }
        *pt_it =NULL;
    }
    m_drawTracks.clear();
}

void RPData::reduceLayerTracks()
{
    if(m_layerTracks.size() > 2)
    {
        LayerTracks::iterator lit;
        DrawTracks::iterator pt_it;
        lit = m_layerTracks.begin();

        for(pt_it = lit->begin(); pt_it != lit->end(); pt_it++)
        {
            if (*pt_it != NULL)
            {
                delete *pt_it;
            }
            *pt_it =NULL;
        }
        lit->clear();
        m_layerTracks.pop_front();
    }
}

void RPData::buildTracks(Line line)
{
    Vec3D lastTail = m_lastTrackItem.getLastTail();
    Vec3D lastHead = m_lastTrackItem.getLastHead();
    double radius = m_lastTrackItem.getRadius();
    Line lastline = m_lastTrackItem.getLine();
    Line scratch;
    Vec3D thishead = line.getHead();
    Vec3D thisTail = line.getTail();
    DataDrawTrack *ptrack;
    Vec2D vec1,vec2;
    Vec3D interPoint,vrot;
    Vec3D shead, send,sleft,sright;
    Vec3D start1,start2,end1,end2;
    Vec3D curLeft,curRight,lastLeft,lastRight;
    RPDisk disk= RPDisk(Vec3D(),radius);
    short direct = 0;

    if(thishead == lastTail)
    {
        if(m_drawTracks.size() >1 )
        {
            m_drawTracks.pop_back();
            m_drawTracks.pop_back();
        }

        vec1 = Vec2D(line.getVector().getX(),line.getVector().getY());
        vec2 = Vec2D(lastline.getVector().getX(),lastline.getVector().getY());

        if(vec1.Cross(vec2)==0)
        {
            Line newLine = Line(lastline.getHead(),line.getTail());
            //body
            newLine.getCorners(start1,start2,end1,end2,radius);
            ptrack = new TrackLine(start1,start2,end1,end2);
            m_drawTracks.push_back(ptrack);
            //tail
            send = newLine.getTail();
            newLine.getVertials(sleft,sright);
            ptrack = new TrackTerminal(send,sright,radius);
            m_drawTracks.push_back(ptrack);

            m_lastTrackItem.setLine(newLine);
        }
        else
        {
            line.getCorners(start1,start2,end1,end2,radius);
            curLeft  = start1;
            curRight = start2;

            vec1 =m_lastTrackItem.getEndScratch(lastLeft,lastRight);
            interPoint = disk.getInternalCenter(lastHead,lastTail,thisTail,vrot);
            vec2 = Vec2D(vrot.getX(),vrot.getY());
            if(vec1.Dot(vec2) < 0)    //arc on the left.
            {
                shead = lastLeft;
                send = curLeft;

                end1 = lastLeft;
                end2 = interPoint;

                scratch = Line(interPoint,curLeft);
                direct = -1;
            }
            else      //arc on the right
            {
                shead = lastRight;
                send = curRight;

                end1 = interPoint;
                end2 = lastRight;

                scratch = Line(curRight,interPoint);
                direct = 1;
            }

            //body
            m_lastTrackItem.getScratch(start1,start2);
            ptrack = new TrackLine(start1,start2,end1,end2);
            m_drawTracks.push_back(ptrack);
            //elbow
            ptrack = new TrackElbow(shead,send,interPoint,lastTail,radius,direct);
            m_drawTracks.push_back(ptrack);

            send = line.getTail();
            line.getVertials(sleft,sright);
            //body
            line.getCorners(start1,start2,end1,end2,radius);
            start1 = scratch.getTail();
            start2 = scratch.getHead();
            ptrack = new TrackLine(start1,start2,end1,end2);
            m_drawTracks.push_back(ptrack);
            //tail
            ptrack = new TrackTerminal(send,sright,radius);
            m_drawTracks.push_back(ptrack);
            m_lastTrackItem.setValue(line,scratch);
        }

    }
    else
    {
        shead = line.getHead();
        send = line.getTail();
        line.getVertials(sleft,sright);

        //head
        ptrack = new TrackTerminal(shead,sleft,radius);
        m_drawTracks.push_back(ptrack);
        //body
        line.getCorners(start1,start2,end1,end2,radius);
        ptrack = new TrackLine(start1,start2,end1,end2);
        m_drawTracks.push_back(ptrack);
        //tail
        ptrack = new TrackTerminal(send,sright,radius);
        m_drawTracks.push_back(ptrack);

        scratch = Line(start2,start1);
        m_lastTrackItem.setValue(line,scratch);
    }
}

void RPData::buildArc(Vec3D send)
{
    DataDrawTrack *ptrack;
    Vec3D start,center;
    double radian,radius,trackWidth;
    getArc(start,center, radian, radius,trackWidth);
    ptrack = new TrackArc(start,send,center,radius,radian,trackWidth);
    m_drawTracks.push_back(ptrack);
}

void RPData::setFittingTrack(bool bs)
{
    b_fittingTrack = bs;
}

bool RPData::isFittingTrack()
{
    return b_fittingTrack;
}

void RPData::setSimu(SIMURP::GTMock *sim)
{
    rp_sim = sim;
}

void RPData::setControl(BuildRP::RPControl* ctl)
{
    rp_ctl = ctl;
}

BuildRP::RPControl* RPData::getControl()
{
    return rp_ctl;
}

double RPData::getCurSimZ()
{
    return m_SlcSlices.at(m_curSimLevel).minZLevel;
}

//!syn
int RPData::updateSimu()
{
    if(rp_sim == NULL || rp_ctl == NULL)
        return -1;


    int i = 0;
    int curlevel,slcSize;
    double stepZ,curZ;

    rp_ctl->m_mutex.lock();

    curlevel = m_curSimLevel;
    slcSize = m_SlcSlices.size();
    curZ = rp_ctl->getPrintZ();
    stepZ = m_maxLayerLevel;

    //<debug
//    QString str = QObject::tr("real Z is ");
//    str.append(QString::number(curZ));
//    str.append(QObject::tr("simu Z is "));
//    str.append(QString::number(m_SlcSlices.at(m_curPrintLevel).minZLevel));
//    qDebug()<<str<<endl;//>debug

    if(curZ >= stepZ)
    {
        m_curPrintLevel = (int)getMaxLevel()-1;
    }
    else
    {
        for(i = 0; i < slcSize; i++)
        {
           stepZ = m_SlcSlices.at(i).minZLevel;
           if( curZ <= stepZ)
           {
               break;
           }
        }
        m_curPrintLevel = i;
    }

    rp_ctl->m_mutex.unlock();

    //qDebug()<<"m_curPrintLevel"<<m_curPrintLevel;
    if(curlevel < m_curPrintLevel)
    {
        return SIMU_SLOW;
    }
    else if(curlevel > m_curPrintLevel)
    {
        return SIMU_FAST;
    }

    return SIMU_SAME;
}

 void RPData::increaseSimuLevel()
 {
    m_curSimLevel++;
    if(m_curSimLevel >= (int)m_maxLevel)
    {
        m_curSimLevel = m_maxLevel -1;
    }
 }

 void RPData::syntoPrinting()
 {
    int stype = updateSimu();

    if(stype == DataRP::SIMU_SLOW)
    {
        rp_sim->adjustSpeed(m_rate);
    }
    else if(stype == DataRP::SIMU_FAST)
    {
        rp_sim->adjustSpeed(1/m_rate);
    }
    else
    {
        rp_sim->adjustSpeed(1);
    }

    //is to stop layer.
    checkStopPoint();
 }

//!transMat
void RPData::setTransMat(Matrix4X4 stransMat)
{
    m_transMat = stransMat;
    qDebug()<<"transMat"<<endl;
}

Matrix4X4 RPData::getTransMat()
{
    return  m_transMat;
}

Matrix4X4 RPData::getSampleTransMat()
{
   return m_transMat;
}

void RPData::alignModel2Measure()
{
    if(m_mesh == NULL)
        return;

    Matrix4X4 compMat;
    if(isCoordCompensation())
    {
        double rotz,transx,transy;
        //ui_gl->getCompensationParam(rotz,transx,transy);
        compMat = compMat.rotZ(rotz);
        compMat = compMat.translate(transx,transy,0);
    }


    TriangleMesh::VertexIterator vIt = m_mesh->beginVertices();
    Vec3D pos = Vec3D( vIt->getX(),vIt->getY(),vIt->getZ());
    Vec3D target = m_transMat *pos;

    if(isCoordCompensation())
        target = compMat * target;

    double minmax[3][2];
    for(int i = 0; i < 2; i++)
    {
        minmax[0][i] = target.getX();
        minmax[1][i] = target.getY();
        minmax[2][i] = target.getZ();
    }

    for(vIt=m_mesh->beginVertices();vIt!=m_mesh->endVertices();++vIt)
    {
         pos = Vec3D( vIt->getX(),vIt->getY(),vIt->getZ());
         target = m_transMat *pos;

         if(isCoordCompensation())
             target = compMat * target;

         vIt->setX(target.getX());vIt->setY(target.getY());vIt->setZ(target.getZ());

         if( vIt->getX() < minmax[0][0])
             minmax[0][0] =  vIt->getX();

         if( vIt->getX() > minmax[0][1])
             minmax[0][1] =  vIt->getX();

         if( vIt->getY() < minmax[1][0])
             minmax[1][0] =  vIt->getY();

         if(vIt->getY() > minmax[1][1])
             minmax[1][1] = vIt->getY();

         if( vIt->getZ() < minmax[2][0])
             minmax[2][0] =  vIt->getZ();

         if( vIt->getZ() > minmax[2][1])
             minmax[2][1] =  vIt->getZ();
    }

//     for(vIt=m_mesh->beginVertices();vIt!=m_mesh->endVertices();++vIt)
//     {
//         pos = Vec3D( vIt->getX(),vIt->getY(),vIt->getZ());
//         vIt->setX(pos.getX());vIt->setY(pos.getY());
//         vIt->setZ(pos.getZ() -  minmax[2][0]);
//     }

    m_mesh->calFacesZ();

    m_mesh->m_bbox.setBounding(minmax[0][0],minmax[0][1],minmax[1][0],
                             minmax[1][1],minmax[2][0],minmax[2][1]);
}

void RPData::readTransMat()
{
   Vec4D vec[4];
   QString filename= QDir::currentPath()+"/../share/mposmat.txt";
   QFile posfile(filename);
   if(!posfile.open(QIODevice::ReadOnly | QIODevice::Text))
   {
        QMessageBox::critical(0,QObject::tr("File cannot open!"),filename);
   }
   QTextStream txtInput(&posfile);
   QString lineStr;
   QStringList strList;
   m_transMat.loadIdentity();
   m_transMat.getRows(vec);
   double value[4] = {0,0,0,0};

   int numraw = 0;
   while(!txtInput.atEnd())
   {
       lineStr = txtInput.readLine();
       strList = lineStr.split(QRegExp("\\s+"));
       if(strList.size() < 4)
       {
            QMessageBox::warning(0,QObject::tr("read measure possition erro"),"filename","is less than 4");
       }

       for(int i = 0; i < 4; i++)
       {
          value[i] = strList.at(i).trimmed().toDouble();
       }
       vec[numraw].setValue(value);
       numraw++;
       if(numraw > 3)
           break;
   }
   m_transMat.setValue(vec);
}

 void RPData::resetTransMat()
 {
      m_transMat.loadIdentity();
 }

void RPData::animateShowLayers()
{
    ui_data->animationSlideLayer();
}

void RPData::vertifyMeasurePos(int index)
{
    Vec3D pos;
    m_measure->getTransMeasurePos(pos,index);
    rp_ctl->move2MeasurePos(pos);
}


void RPData::setCoordCompensation(bool bcomp)
{
    b_coordCompensation = bcomp;
}

bool RPData::isCoordCompensation()
{
    return b_coordCompensation;
}
