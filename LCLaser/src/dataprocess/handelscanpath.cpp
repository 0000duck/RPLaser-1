/***************************************************************************
                          handelscanpath.cpp  -  description
                             -------------------
    begin                : Jul. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "handelscanpath.h"
#include "datathread.h"
#include <QTime>

using namespace DataRP;

struct CircumDisk: public RPDisk
{
    enum CircleType
    {
        NO_BUILD = 0,
        CANNOT_BUILD,
        ARC_MATURE,
        ARC_IMMATURE
    };

    CircumDisk():RPDisk(),status(NO_BUILD),counter(0){}

    ~CircumDisk(){}

    CircumDisk(Vec3D scenter,double sradius):
        RPDisk(scenter,sradius),status(NO_BUILD),counter(0)
    {}

    CircumDisk(Line &line1, Line &line2)
    {
        status = build(line1,line2);
        cstart = line1.getHead();
        cend = line2.getTail();
    }

    void clear()
    {
        status = NO_BUILD;
        counter = 0;
    }

    int build(Line &line1, Line &line2)
    {
        Vec3D vec,direct2center,tcenter;
        double bevel_length,line_length,diff_length,half_length,value,theta,alpha;
        double dot, cross, arccos,tradius;

        if(line1.getTail()!=line2.getHead())
        {
            qDebug()<<"two line not linked"<<endl;
            return CANNOT_BUILD;
        }

        //not same long.
        bevel_length = line1.getLength();
        line_length = line2.getLength();
        diff_length = bevel_length - line_length;

        if(diff_length < 0)
            diff_length = - diff_length;

        if( diff_length > bevel_length*0.1)
        {
            return CANNOT_BUILD;
        }

        //is a sharp angle.
        dot = line1.Dot(line2);
        if(dot <= 0 )
        {
            return CANNOT_BUILD;
        }

        //obtuse angle limit.
        cross = line1.Cross(line2);
        if(cross == 0)//parallel.
        {
            return CANNOT_BUILD;
        }
        else
        {
            arccos = acos(-dot);

            if(arccos < 0.6*M_PI)
                return CANNOT_BUILD;
        }

        //calculate
        vec = line2.getTail() - line1.getHead();
        half_length = vec.Length()/2.0;
        vec.Normalize();
        //find direction to center.
        if(cross > 0)
        {
            direction = 1;
            direct2center = vec.VerticalXYLeft();
        }
        else
        {
            direction = -1;
            direct2center = vec.VerticalXYRight();
        }

        value = half_length/bevel_length;

        if(value > 1)
        {
            //qDebug()<<"sine is out of range."<<endl;
            return CANNOT_BUILD;
        }

        //theta = asin(value);
        //alpha = M_PI - 2.0*theta;
        theta = acos(value);
        alpha = 2*theta;

        value = sin(alpha);
        if(value == 0)
        {
            qDebug()<<"angle is zero"<<endl;
            return CANNOT_BUILD;
        }

        tradius = half_length/value;
        tcenter = line2.getHead() + direct2center*tradius;
        setRadius(tradius);
        setCenter(tcenter);

        counter = 0;

        arcType = RP_ARCP;
        return ARC_IMMATURE;
    }

    //!extend circle.
    bool hasPoint(Vec3D spoint)
    {
        if(status == CANNOT_BUILD)
        {
              return false;
        }

        double diff,cradius;
        Vec3D tpoint = getCenter();
        Vec3D vec = tpoint - spoint;
        double length = vec.Length();
        cradius = getRadius();
        diff = cradius -length;

        //in case of mis-judge.
        if (diff < 0)
            diff = -diff;

        if(diff < 0.05 )//cradius*0.01
        {
            counter++;
            cend = spoint;
            return true;
        }

        return false;
    }

    //!can be push.
    bool isMature()
    {

        if(status == CANNOT_BUILD)
        {
              return false;
        }
        else
        {
            if(counter > 0)
            {
                status = ARC_MATURE;
                return true;
            }
        }
        return false;
    }

    bool isMajorArc()
    {
        if(cstart == cend)
        {
            arcType = RP_ARC;
            radian = 2*M_PI;

            if(direction == -1)
                radian = -radian;

            return true;
        }

        Vec3D tcenter = getCenter();
        Vec3D vec1,vec2,vstart,vend;
        vec1 = cstart - tcenter;
        vec2 = cend - tcenter;
        vec1.Normalize();
        vec2.Normalize();

        if(direction == 1)
        {
            vstart = vec1;
            vend = vec2;
        }
        else
        {
            vstart = vec2;
            vend = vec1;
        }

        if(vstart.Sine(vend) < 0)
        {
            setRadius(-getRadius());
            return true;
        }

        return false;
    }

    RPprintItem getPrintItem()
    {
        isMajorArc();
        RPprintItem item;

        if(arcType == RP_ARCP)
        {
            item = RPprintItem(cend,getRadius(),direction,getCenter());
        }
        else if(arcType == RP_ARC)
        {
            item = RPprintItem(getRadius(),radian,getCenter(),cend);
        }

        return item;
    }

    bool isBuilt()
    {
        if(status == NO_BUILD )
            return false;

        return true;
    }

    private:
        int      status;
        unsigned counter;
        Vec3D    cstart;
        Vec3D    cend;
        short    direction;
        int      arcType;
        double   radian;
};

unsigned HandelScanPath::m_curCoursor=0;

HandelScanPath::HandelScanPath(RPData *rpdata,QObject *parent):
                    QObject(parent),
                   rp_data(rpdata)
{
}

HandelScanPath::~HandelScanPath()
{
}

void HandelScanPath::handleResults(QString str)
{
    rp_data->terminateSimuTimer();
    qDebug()<<str<<endl;
}

void HandelScanPath::dealOuntlinesandPaths(HandelScanPath *dealpath)
{
    dealpath->genOutlinesandPaths(true);
}

void HandelScanPath::setHasPath(bool has)
{
    b_hasPath = has;
}

void HandelScanPath::startGenScanPath()
{
    rp_data->setStopGenPath(false);
    rp_data->b_canLoad = false;
    DataThread *dataProcessThread = new DataThread();
    dataProcessThread->setProcess(&dealOuntlinesandPaths,this,QObject::tr("--------operating ok"));
    connect(dataProcessThread, SIGNAL(resultReady(QString)), this, SLOT(handleResults(QString)));
    connect(dataProcessThread, &DataThread::finished, dataProcessThread, &QObject::deleteLater);
    dataProcessThread->start();
    //dealOuntlinesandPaths(this);
}

void HandelScanPath::genCurrentLayerOutlines()
{
    m_curLayer = rp_data->getCurrentLayer();
    switchFittingTrack(rp_data->isFittingTrack());
    m_numboutlines = rp_data->getNumberOutlines();
    clearPath();
    genOutlines();

    if(b_hasPath)
        genCurLayerPath(rp_data->getPathInterval(),rp_data->getCompenRadius());

    setCurlayerPathOK();
    rp_data->refresh();
}

void HandelScanPath::genOutlinesandPaths(bool isAllLayers)
{
    int maxlayer;
    rp_data->dataProcessPerc = 0.0f;
    switchFittingTrack(rp_data->isFittingTrack());
    m_numboutlines = rp_data->getNumberOutlines();
    if(!isAllLayers)  //just current layer.
    {
        m_curLayer = rp_data->getCurrentLayer();
        genOutlines();

        clearPath();
        if(b_hasPath)
            genCurLayerPath(rp_data->getPathInterval(),rp_data->getCompenRadius());

        setCurlayerPathOK();
    }
    else                     //all the layer.
    {
        maxlayer = rp_data->getMaxLevel();

        for(m_curLayer = 0; m_curLayer < maxlayer; m_curLayer++)
        {
            //termination process thread.
            if(rp_data->isStopGenPath())
                break;

            genOutlines();

            clearPath();
            if(b_hasPath)
            {
                  rp_data->applyCutPlaneRot(rp_data->getCutplaneRot());
                  genCurLayerPath(rp_data->getPathInterval(),rp_data->getCompenRadius());
            }

            rp_data->dataProcessPerc = (float)m_curLayer/maxlayer;
            setCurlayerPathOK();
        }
    }
    rp_data->dataProcessPerc = 1.0f;
    rp_data->b_canLoad = true;
    //rp_data->animateShowLayers();
    //rp_data->refresh();
}

void HandelScanPath::setCurlayerPathOK()
{
    rp_data->m_SlcSlices.at((unsigned)m_curLayer).setPathOk();
}

void HandelScanPath::genOutlines()
{
    //qDebug()<<"the layer "<<m_curLayer<<" outline size is "<<rp_data->getTheLayerOutlinesAmount(m_curLayer)<<endl;
    if( rp_data->getTheLayerOutlinesAmount(m_curLayer) == 0)
    {
        genOuterOutlines();
    }
    genInterOutlines(m_numboutlines);
}

//! generate outermost outline.
void HandelScanPath::genOuterOutlines()
{
    rp_data->clearTheLayerOutlines(m_curLayer);
    RPboundaries boundaries;

    m_offset = rp_data->getCompenRadius();
    genOutline(rp_data->getTheLayerOuterBoundaries(m_curLayer),boundaries);
    rp_data->addTheLayerBoundaries(m_curLayer,boundaries);
}

void HandelScanPath::genInterOutlines(int number)
{
    if(number < 1)
    {
         rp_data->clearTheLayerOutlines(m_curLayer);
    }

    int ccurr = (int)rp_data->getTheLayerOutlinesAmount(m_curLayer);

    if(number == ccurr)
        return;

    int diff;

    if(number < ccurr)
    {
        diff = ccurr - number;

        for(int i=0; i < diff; i++)
        {
            rp_data->popbackTheLayerBoundaries(m_curLayer);
        }
    }
    else
    {
        diff = number - ccurr;
        RPboundaries boundaries;
        for(int i=0; i<diff; i++)
        {
            boundaries.clear();
            m_offset = rp_data->getPathInterval();
            genOutline(rp_data->getTheLayerlastBoundaries(m_curLayer),boundaries);
            rp_data->addTheLayerBoundaries(m_curLayer,boundaries);
        }
    }
}

void HandelScanPath::genPrintTrack(RPcontour &contour)
{
    if(b_fittingTrack)
        genArcPrintTrack(contour);
    else
        genSimpleTrack(contour);
}

void HandelScanPath::genArcPrintTrack(RPcontour &contour)
{
    RPlines2D linelist;
    RPprintItem sItem;
    RPprintItem tempItem;
    unsigned listSize;
    unsigned curCursor = 0;
    unsigned preCursor,pre_cursor;
    bool bBuildCircle;
    CircumDisk sCircle;
    Vec3D tpoint;
    Line line1,line2;

    std::copy(contour.lineList.begin(),contour.lineList.end(), std::back_inserter(linelist));
    listSize = linelist.size();
    LoopOperate lop = LoopOperate(listSize);

    QTime wtime;
    wtime.start();
    //qDebug()<<"time of this day"<<wtime.msecsSinceStartOfDay()<<endl;
    //srand((int)wtime.msecsSinceStartOfDay());
    curCursor = rand()%listSize;
    if(m_curCoursor == curCursor)
    {
        curCursor = lop.loopAdd(curCursor,1);
    }
    m_curCoursor = curCursor;

    preCursor = lop.loopMinus(curCursor,1);
    tpoint = linelist[preCursor].getHead().toVec3D(0.0);
    sItem = RPprintItem(tpoint);
    contour.printTrack.push_back(sItem);

    for(unsigned cursor = curCursor; cursor != preCursor; cursor = lop.loopAdd(cursor,1))
    {
        bBuildCircle = true;
        if(sCircle.isBuilt())
        {
            tpoint = linelist[cursor].getTail().toVec3D(0.0);
            if(sCircle.hasPoint(tpoint))
            {
                continue;
            }
            else
            {
                if(sCircle.isMature())
                {
                     contour.printTrack.push_back(sCircle.getPrintItem());
                     bBuildCircle = false;
                     sCircle.clear();
                }
                else
                {
                     contour.printTrack.push_back(tempItem);
                }
            }//else
        }//if

        if(bBuildCircle)
        {
            tpoint = linelist[cursor].getHead().toVec3D(0.0);
            tempItem = RPprintItem(tpoint);

            pre_cursor = lop.loopMinus(cursor,1);
            line1 = linelist[pre_cursor].line23D(0.0);
            line2 = linelist[cursor].line23D(0.0);
            sCircle = CircumDisk(line1,line2);
        }

    }//for

    if(sCircle.isBuilt())//donnot no build
    {
        if(sCircle.isMature())
        {
             contour.printTrack.push_back(sCircle.getPrintItem());
        }
        else
        {
             contour.printTrack.push_back(tempItem);
             tpoint = linelist[preCursor].getHead().toVec3D(0.0);
             sItem = RPprintItem(tpoint);
             contour.printTrack.push_back(sItem);
        }
    }
    else
    {
        tpoint = linelist[preCursor].getHead().toVec3D(0.0);
        sItem = RPprintItem(tpoint);
        contour.printTrack.push_back(sItem);
    }

//    qDebug()<<"print track OK"<<endl;

}

void HandelScanPath::genSimpleTrack(RPcontour &contour)
{
    RPlines2D linelist;
    RPprintItem sItem;
    unsigned listSize;
    unsigned preCursor,curCursor;
    Vec3D tpoint;

    std::copy(contour.lineList.begin(),contour.lineList.end(), std::back_inserter(linelist));
    listSize = linelist.size();
    LoopOperate lop = LoopOperate(listSize);

    QTime wtime;
    wtime.start();
    srand(wtime.msecsSinceStartOfDay());
    curCursor = rand()%listSize;
    if(m_curCoursor == curCursor)
    {
        curCursor = lop.loopAdd(curCursor,1);
    }
    m_curCoursor = curCursor;


    preCursor = lop.loopMinus(curCursor,1);

    tpoint = linelist[preCursor].getHead().toVec3D(0.0);
    sItem = RPprintItem(tpoint);
    contour.printTrack.push_back(sItem);

    for(unsigned cursor = curCursor; cursor != preCursor; cursor = lop.loopAdd(cursor,1))
    {
        tpoint = linelist[cursor].getHead().toVec3D(0.0);
        sItem = RPprintItem(tpoint);
        contour.printTrack.push_back(sItem);
    }

    tpoint = linelist[preCursor].getHead().toVec3D(0.0);
    sItem = RPprintItem(tpoint);
    contour.printTrack.push_back(sItem);

//    qDebug()<<"print track OK"<<endl;
}

void HandelScanPath::switchFittingTrack(bool bs)
{
    b_fittingTrack = bs;
}

void HandelScanPath::clearPath()
{
    rp_data->clearTheLayerPaths(m_curLayer);
}

bool HandelScanPath::canbePath(PathPoint &phead, PathPoint &ptail)
{
    double len;
    Vec3D headvec = phead.clockDirection;
    Vec3D tailvec = ptail.clockDirection;

    Vec3D head = Vec3D(phead.vertex.getX(),phead.vertex.getY(),0);
    Vec3D tail = Vec3D(ptail.vertex.getX(),ptail.vertex.getY(),0);
    Vec3D normal = tail-head;
    len = normal.Length();

    Vec3D head_direct = tail - head;
    Vec3D tail_direct = head -tail;

    head_direct.Normalize();
    tail_direct.Normalize();

    if(head_direct.Dot(headvec) > 0 && tail_direct.Dot(tailvec) > 0 && len >rp_data->getCompenRadius()*2) //draw in the inter-modle and the length is enough.
    {
      return true;
    }

    return false;
 }

//! gen parallel paths.
void HandelScanPath::genCurLayerPath(double spathInterval,double scompensateRadius)
{
     clearPath();
     rp_data->setCutPlanePoint();
     RPline2D path;
     PathContainer container;//paths are arranged with the order of cutplanes from begin to end.
     bool breached;

     rp_data->clearTheLayerPaths(m_curLayer); //clear privous path.

     RPboundaries::iterator itbond,itbegin,itend;
     RPpolygon::iterator itver;
     PathPoints::iterator itpath;

     unsigned steps,index = 0;
     double dist,dotvalue;
     Vec3D  wing[2];
     Vec3D  normal = rp_data->getCutPlaneNormal();
     Vec3D  basePoint = rp_data->getCutPlanePoint();
     Vec3D  planePeak = rp_data->getCutPlanePeak();
     Vec3D  planePoint,lhead,ltail,crossPoint,linePeak,lray,linevc;
     Plane  cplane = Plane(normal,basePoint);
     //set far point.
     normal.VerticalXY(wing[0],wing[1]);
     linePeak = basePoint + wing[0]*LARGE;
     PathPoint::setFarpoint(linePeak); //far point is at left.

     dist = cplane.calcDistance(planePeak);
     //get the intersection m_curLayer of a layer.
     steps = ceil(dist / spathInterval);
     container.resize(steps+3);

     basePoint += (spathInterval + scompensateRadius)*normal;

    //iterate all boundary edge.
    if(rp_data->m_SlcSlices.at(m_curLayer).outlines.size() == 0)
    {
        if(rp_data->m_SlcSlices.at(m_curLayer).outerBoundaries.size() == 0)
        {
            return ;
        }
        itbegin = rp_data->m_SlcSlices.at(m_curLayer).outerBoundaries.begin();
        itend =  rp_data->m_SlcSlices.at(m_curLayer).outerBoundaries.end();
    }
    else
    {
        itbegin = (rp_data->m_SlcSlices.at(m_curLayer).outlines.end()-1)->begin();
        itend =  (rp_data->m_SlcSlices.at(m_curLayer).outlines.end()-1)->end();
    }


    for(itbond = itbegin; itbond != itend; itbond++)
    {
        for(itver = itbond->vertList.begin()+1; itver != itbond->vertList.end(); itver++)
        {
            lhead = Vec3D((itver-1)->getX(),(itver-1)->getY(),0);
            ltail = Vec3D(itver->getX(),itver->getY(),0);
            breached = false;
            index = 0;

            //cutplanes cutting the edge.
            while(index < steps )
            {
                //cutplane moving on according to step. in factor doesn't help much. finally,judge the cut by two side segment.
                planePoint = basePoint + index*spathInterval*normal;
                cplane.setPoint(planePoint);
                crossPoint = Vec3D(0.0,0.0,0.0);


                if(!cplane.intersect(lhead,ltail,crossPoint)) // cutplane is not intersect with edge.
                {
                   //if distance to cplane is too near.
                   //remeber this segment as nearoutline. put into pathContainer.

                    if(breached)  // if the edge has already been cutted, then break. means the edge should be continously cutting, cannot skip.
                        break;
                    else
                    {
                        index++;
                        continue;
                    }
                }
                else                                        //cutplane cutting the edge.
                {
                    linevc = ltail - lhead;
                    lray = linevc.VerticalXYLeft(); //left side of the edge. if the edge is outer contour, it point to inside. if the edge is inter contour, it point to outer side.

                    dotvalue = lray.Dot(wing[0]);//left

                    if(dotvalue > 0)
                    {
                      lray = wing[0];
                    }
                    else
                    {
                      lray = wing[1];
                    }

                    //beveling
                    //if(absf(dotvalue) > 0.52532198881773)
                    //put the segment to point's partner line.

                    PathPoint PathPoint(crossPoint.getX(),crossPoint.getY(),lray);
                    container[index].push_back(PathPoint);

                    breached = true;
                }

                index++;
             }//while
         }//vertex
    }//outerBoundaries

    //arragements every points on a cutplane.
    for(unsigned i = 0; i < steps; i++)
    {
        if(container[i].size() < 2)
           continue;
        //farpoint for compary.
        sort(container[i].begin(),container[i].end(),PathPoint::compare);

        if(i%2 != 0) // move back and foth mode.
            reverse(container[i].begin(),container[i].end());

        //every cut line.
        for(itpath = container[i].begin()+1; itpath != container[i].end();itpath++)
        {
            if(!canbePath(*(itpath -1),*itpath))
            {
                continue;
            }

            //move end points with compensation.
            path.setHead((itpath-1)->vertex,(itpath-1)->clockDirection,spathInterval);
            path.setTail(itpath->vertex,itpath->clockDirection,spathInterval);

            rp_data->m_SlcSlices.at(m_curLayer).paths.push_back(path);
        }
    }
  //  qDebug()<<"path size "<<rp_data->m_SlcSlices.at(m_curLayer).paths.size()<<endl;
}

