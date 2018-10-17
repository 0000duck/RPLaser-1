/***************************************************************************
                          handelBallOutline.cpp  -  description
                             -------------------
    begin                : Jul. 12 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "handelBallOutline.h"

using namespace DataRP;

HandelBallOutline::HandelBallOutline(RPData *rpdata):
                   HandelScanPath(rpdata)
{
    b_simuBall = rpdata->isDrawProcess();
}

HandelBallOutline::~HandelBallOutline()
{
}

void  HandelBallOutline::getAniCommdsSize()
{
    unsigned commdsSize =  rp_data->m_simuBall->getCommdsSize();
    rp_data->changeCommdsSize(commdsSize);
}

double HandelBallOutline::disForward2Pos(unsigned sindex)
{
    double length = 0;
    unsigned i;
    unsigned start = m_cursor;
    unsigned send  = sindex;
    for(i = start; i != send;)
    {
        length += m_linelist[i].getLength();
        i = m_lop.loopAdd(i,1);
    }
    qDebug()<<"length:  "<<length<<endl;
    return length;
}

double HandelBallOutline::distBackward2Pos(unsigned sindex)
{
    double length = 0;
    unsigned i;
    unsigned start = sindex;
    unsigned send  = m_precursor;
    for(i = start; i != send;)
    {
        length += m_linelist[i].getLength();
        i = m_lop.loopAdd(i,1);
    }
    qDebug()<<"length:  "<<length<<endl;
    return length;
}

//!
//! \brief generate ball outline
//! \param offset the distance from neighbour outline.
//! \param inputboundaries  the referenced neighbour outline.
//! \param outputboundaries  new outlines.
//!
void HandelBallOutline::genOutline(RPboundaries &inputboundaries,RPboundaries &outputboundaries)
{
    RPcontour contour;
    RPboundaries::iterator itbond;
    RPboundlist::iterator itlist;

    m_circum = 2*M_PI*m_offset;
    m_curZ = rp_data->getTheLayerZ(m_curLayer);

    unsigned  cnumb = 0;
    unsigned  boundSize = inputboundaries.size();

    if(b_simuBall)
    {
        rp_data->m_simuBall->clear();
    }

    int result;
    m_boundlist.clear();
    m_boundrefer.clear();

    for(itbond = inputboundaries.begin(); itbond != inputboundaries.end();itbond++)
    {
        m_boundlist.push_back(*itbond);
        m_boundrefer.push_back(*itbond);
    }

    //iterate every boundary.
    for(itlist =  m_boundlist.begin(); itlist !=  m_boundlist.end(); itlist++)
    {
        if(rp_data->isStopGenPath())
            break;

        //if necessary to check this contour.
        if(cnumb < boundSize)
        {
            //in case of divided.
            if( m_boundrefer[cnumb].isJump())
            {
                cnumb++;
                continue;
            }
            m_boundrefer[cnumb].switchJump(true);//alrady checked.
        }


        //clear linelist for this time use.
        m_linelist.clear();
        contour.clear();

        std::copy(itlist->lineList.begin(),itlist->lineList.end(),
                        std::back_inserter(m_linelist));

        m_limit = m_linelist.size();
        m_lop = LoopOperate(m_limit);
        result = rollBall(contour);  //cuibin key.

        if(result == CHECK_LOOPOVER)
        {
            contour.calnumVerts();
            genPrintTrack(contour);
            outputboundaries.push_back(contour);
        }
        else if(result == CHECK_OTHERCROSS)
        {
            qDebug()<<"collide other new out lines"<<endl;
            m_contour1.calnumVerts();
            m_boundlist.push_back(m_contour1);
        }
        else if(result == CHECK_SELFCROSS)
        {
            qDebug()<<"self new outlines"<<endl;
            m_contour1.calnumVerts();
            m_contour2.calnumVerts();
            m_boundlist.push_back(m_contour1);
            m_boundlist.push_back(m_contour2);
        }
        else
        {
            qDebug()<<"can not forming a outline"<<endl;
            //inputboundaries.push_back(new lines);
//            if(contour.vertList.size()> 0)
//            {
//                contour.calnumVerts();
//                outputboundaries.push_back(contour);
//            }
        }

        if(b_simuBall)
        {
            rp_data->m_simuBall->endOneLoop();
        }

        cnumb++; //reference boundary.
    }//iterate every boundary.

    if(b_simuBall)
    {
        getAniCommdsSize();
    }
}

//!
//! \brief generate new contour.
//! \param contour   new contour.
//! \return
//!
int HandelBallOutline::rollBall(RPcontour &contour)
{
    //incase of too short.
    if(m_linelist.size() < 3)
        return CHECK_TOOSMALL;

//    qDebug()<<"m_limit = "<<m_limit<<endl;
    m_scursor = 0;
    m_cursor = 0;
    m_loopcount = 0;
    m_precursor = m_limit-1;

    m_neigbourRange = m_limit * 0.125;

    if(m_neigbourRange > 15)
        m_neigbourRange = 15;
    if(m_neigbourRange < 4)
        m_neigbourRange = 3;

    //qDebug()<<"line list size "<<m_limit<<" neigbour range is "<<m_neigbourRange<<endl;

    //animation.
    m_v2base = Vec2D();

    int result = CHECK_NORMAL;
    RollBalls::iterator ballit;

    //generate all balls.
    if(!genAllBalls())
    {
        qDebug()<<"the boundary is not close"<<endl;
        return CHECK_NOCLOSE;
    }

    m_disloop = 0;
    b_exitloop = false;
    //loop for collision.
    while(result == CHECK_NORMAL )
    {
        result = ballonLine();
    }

    //exit
    if(result != CHECK_LOOPOVER)
    {
        return result;
    }

    //clean up centers.
    for(ballit = m_balls.begin(); ballit != m_balls.end(); )
    {
        if(ballit->isErase())
        {
            ballit = m_balls.erase(ballit);
        }
        else
        {
            ballit++;
        }

        if(ballit == m_balls.end())
        {
            break;
        }
    }

    if(m_balls.size() < 3)
    {
        qDebug()<<"the balls are too few"<<endl;
        return CHECK_TOOSMALL;
    }


    //push centers to contour.
    for(ballit = m_balls.begin(); ballit != m_balls.end(); ballit++)
    {
        contour.vertList.push_back(ballit->center);
    }

    contour.vertList.push_back(m_balls.begin()->center);
    return CHECK_LOOPOVER;
}

//!
//! \brief generate all balls in lines coner.
//! \param offset radius
//!
 bool  HandelBallOutline::genAllBalls()
 {
       unsigned preit,curit;
       Vec2D center,intersec1,intersec2;
       RPline2D uline,vline;
       int result = 0;
       unsigned i = 0;

       RP2DDisk ball = RP2DDisk(Vec2D(),m_offset);
       RollBall rollball;

       m_balls.clear();

       for(i = 0; i < m_limit; i++)
       {
           if(i == 0)
           {
               preit = m_limit-1;
               curit = 0;
           }
           else
           {
               preit = i-1;
               curit = i;
           }


           uline = m_linelist[preit];
           vline = m_linelist[curit];
           //get ball center.
           result = ball.getCenter(uline,vline,center,intersec1,intersec2);

           //not a close boundary.
           if(result == -1)
           {
               qDebug()<<"two line not contact."<<endl;
               return false;
           }

           rollball = RollBall(center,ball.getRadius(),intersec1,intersec2,result);
           m_balls.push_back(rollball);

           //animation.
           rp_data->m_simuBall->addBall(rollball,m_curZ);
           rp_data->m_simuBall->addCursor(vline,m_curZ,0.5);
       }
       //animation.
       rp_data->m_simuBall->addBallsOK(m_limit-1);
       return true;
 }

//loop
int HandelBallOutline::ballonLine()
{
//    if(rp_data->isStopGenPath())
//        return CHECK_NORMAL;

    unsigned disloop;
    disloop = m_lop.loopMinus(m_cursor,0);
    if(disloop >= m_disloop)
    {
        m_disloop = disloop;
    }
    else
    {
        b_exitloop = true;
    }


    if(b_exitloop)
    {
        if(m_cursor == (unsigned)m_lop.loopAdd(m_precursor,1) )
            return CHECK_LOOPOVER;
    }//else continue as following.

    int result, checked;
    Vec2D center,intersec1,intersec2,node;
    RPline2D uline,vline,terminalLine;
    RP2DDisk ball = RP2DDisk(Vec2D(),m_offset);
    RollBall rollball;

    uline = m_linelist[m_precursor];
    vline = m_linelist[m_cursor];

    //qDebug()<<" precursor "<<m_precursor<<" cursor  "<<m_cursor<<endl;
    //not connect.
    if( m_cursor!= (unsigned)m_lop.loopAdd(m_precursor,1) )
    {
        terminalLine = RPline2D(vline.getTail(),uline.getHead());
        //intersect at correct positon.
        if(terminalLine.Cross(uline) > 0 && vline.Cross(terminalLine) > 0 && uline.Cross(vline) > 0)
        {
            node = uline.intersect(vline);
            uline = RPline2D(uline.getHead(),node);
            vline = RPline2D(node,vline.getTail());

        }
        else  //intersect at error position.
        {
            vline = m_linelist[m_cursor];
            uline = RPline2D(uline.getTail(),vline.getHead());
            m_precursor = (unsigned)m_lop.loopAdd(m_precursor,1);
        }
        //get new center.
        result = ball.getCenter(uline,vline,center,intersec1,intersec2);

        //two line not intersection.
        if(result == -1)
        {
            m_balls[m_cursor].setErase(true);
            rp_data->m_simuBall->deleteBall(m_cursor);
            qDebug()<<" two line are not intersected"<<m_cursor<<" "<<m_precursor<<endl;

            m_precursor = m_cursor;
            m_cursor = m_lop.loopAdd(m_cursor,1);

           return CHECK_NORMAL;
        }

        rollball = RollBall(center,ball.getRadius(),intersec1,intersec2,result);  //shrink generated ball.
        rollball.setStatus(1);
        m_balls[m_cursor] = rollball;

        //animation.
        rp_data->m_simuBall->moveBall(rollball,m_cursor,uline,vline,m_curZ);
    }

     checked = checkSelfcross();

     if(m_scursor != m_cursor)
     {
         m_scursor = m_cursor;
         m_loopcount = 0;
     }
     else
     {
         m_loopcount++;
         if(m_loopcount > 3)
         {
             m_cursor = m_lop.loopAdd(m_cursor,1);
             m_loopcount = 0;
             //qDebug()<<"------stay too long"<<endl;
         }
     }

    return checked;
}

//!
//! \brief RPData::checkSelfcross
//! \return 0: not cross; 1:cross with one side; 2; cross with two side;
//!
int HandelBallOutline::checkSelfcross()
{
    int result = 0;
    //if ball already deleted, go ahead.
    if(m_balls[m_cursor].isErase())
    {
       m_precursor = m_cursor;
       m_cursor = m_lop.loopAdd(m_cursor,1);
       return CHECK_NORMAL;
    }

    //list
    ERList contactList;
    Vec2D intersec1,intersec2;
    unsigned index,hitindex,cursec,presec,start,send;
    int bumptype;
    ERItem item;
    BallItem ballItem;
    ERList::iterator erit;
    ERList::reverse_iterator reit;
    ERListGroup ergp;
    ERListGroup::iterator grit;

    RPline2D uline,vline,contactline,direct1,direct2;
    Tracks contactLines;
    Line line3D;
    uline = m_linelist[m_precursor];
    vline = m_linelist[m_cursor];
    RP2DDisk ball = RP2DDisk(m_balls[m_cursor].center,m_balls[m_cursor].radius);

    if(b_simuBall)
    {
        rp_data->m_simuBall->updateCursor(m_cursor,m_precursor);
    }
    //not concave and dont contact with both sides.           O
    //                                                    --\  /---
    intersec1 = m_balls[m_cursor].intersec1;
    intersec2 = m_balls[m_cursor].intersec2;
    bumptype =  m_balls[m_cursor].bumptype;
    if(bumptype != 1)
    {
        if(!uline.isPointOnLine(intersec1)&&!vline.isPointOnLine(intersec2) )//&& m_balls[m_cursor].getStatus()!=1
        {
            if(m_cursor != (unsigned)m_lop.loopAdd(m_precursor,1)) //not original cursor.
            {
                direct1 = RPline2D(uline.getTail(),intersec1);
                direct2 = RPline2D(intersec2,vline.getHead());

                if(direct1.Dot(uline) > 0 && direct2.Dot(vline) > 0) //normal.
                {
                    m_precursor = m_cursor;
                    m_cursor = m_lop.loopAdd(m_cursor,1);
                    return CHECK_NORMAL;
                }
            }

            start =  m_precursor;
            send  = m_lop.loopAdd(m_cursor,1);

            m_precursor = m_lop.loopMinus(m_precursor,1);
            m_cursor = m_lop.loopAdd(m_cursor,1);

            for(index = start; index != send; )
            {
            m_balls[index].setErase(true);
            //animation
             rp_data->m_simuBall->deleteBall(index);
            index = m_lop.loopAdd(index,1);
            }//for

            return CHECK_NORMAL;
        }
    }

    //cross with others
//    if(isIntsectOther(ball))
//    {
//        return CHECK_OTHERCROSS;
//    }
    //self collision begin-------
    //     mark lines collision with the ball (traversal).
    index = m_lop.loopAdd(m_cursor,1);
    hitindex = m_cursor;
    cursec = m_cursor;
    presec = m_precursor;
    item.clear();
    //find neigbour.
    while(index != m_precursor)
    {
        //ball intersect with outline.
        if(ballInterLine(ball,index,ballItem))
        {
            //not close link with pre one.
            if(index != (unsigned)m_lop.loopAdd(hitindex,1))
            {
                if(item.getLength() > 0)
                {
                    ergp.push_back(item);
                    item.clear();
                }
            }

            if(index == (unsigned)m_lop.loopAdd(m_cursor,1))
            {
                item.setType(ERLIST_CURSOR);
            }
            else if(index == (unsigned)m_lop.loopMinus(m_precursor,1))
            {
                if(item.getType() != ERLIST_CURSOR) //in case a loop.
                {
                    item.setType(ERLIST_PRECURSOR);
                }
            }

            item.erlist.push_back(ballItem);
            hitindex = index;
        }

        index = m_lop.loopAdd(index,1);
    }

    if(item.getLength() > 0)
    {
        ergp.push_back(item);
    }

   //exclude small length line.
    bool befound = false;
    for(grit = ergp.begin(); grit != ergp.end(); grit++)
    {
        if(grit->getLength() > 0.5 *m_limit)
        {
            qDebug()<<"outline too small"<<endl;  //shrink into small area.
            return CHECK_TOOSMALL;
        }

        if(grit->getType() == ERLIST_SELFCROSS)//if not this clear.
        {
            erit = grit->erlist.begin();
            //distance
            if(disForward2Pos(erit->getIndex()) < m_circum)
            {
               grit->setType(ERLIST_CURSOR);
            }

            if(!befound)
            {
                erit = grit->erlist.end()-1;
                //distance
                if(distBackward2Pos(erit->getIndex()) < m_circum)
                {
                   grit->setType(ERLIST_PRECURSOR);
                   befound = true;
                }
            }//if befound

       }//if self cross
    }//for


    //Arrangement self cross.
    befound = false;
    for(grit = ergp.begin(); grit != ergp.end(); grit++)
    {
        if(grit->getType() == ERLIST_CURSOR)
        {
            cursec = (grit->erlist.end()-1)->getIndex();
        }
        else if(grit->getType() == ERLIST_PRECURSOR && !befound)
        {
            presec = grit->erlist.begin()->getIndex();
            befound = true;
        }
        else//cut the line. ERLIST_SELFCROSS
        {
            if(getSelfCutPoint(*grit))
                return CHECK_SELFCROSS;
        }
    }

    //collision end-------

    //in case of two cursor overlap.
    if(cursec == presec)
    {
        qDebug()<<"cursor precursor are same "<<cursec<<endl;
        presec = m_precursor;
    }

    //key, ball index is same as cursor, not precursor.
    //deal with
    if(m_cursor != cursec || m_precursor != presec)
    {
        result++;
        m_precursor = presec;
        m_cursor = cursec;
        rp_data->m_simuBall->updateCursor(m_cursor,m_precursor);
        start = m_lop.loopAdd(presec,1);  //next ball.
        send = m_lop.loopAdd(cursec,1);

        //delete self and  others
        //m_balls[maxsec].setErase(true);
        for(index = start; index != send; )
        {
            m_balls[index].setErase(true);
            //animation
            rp_data->m_simuBall->deleteBall(index);
            index = m_lop.loopAdd(index,1);
        }//for
    }//if

    //***animation contactline*****
   for(reit = contactList.rbegin();reit != contactList.rend();reit++)
   {
       contactline = m_linelist[reit->getIndex()];
       line3D = contactline.line23D(m_curZ);
       contactLines.push_back(line3D);
   }
   rp_data->m_simuBall->addContactLines(contactLines);

    if(result == 0)//move forward.
    {
        m_precursor = m_cursor;
        m_cursor = m_lop.loopAdd(m_cursor,1);
    }
    return CHECK_NORMAL;
}

bool HandelBallOutline::ballInterLine(RP2DDisk &ball,unsigned currIndex,BallItem &Item)
{
    int endname;
    Vec2D intsecPos;

    if(ball.isIntersect(m_linelist[currIndex],endname,intsecPos))
    {
        Item = BallItem(currIndex,endname,intsecPos);
        return true;
    }

    return false;
}

void HandelBallOutline::unlashSelfLoop(int result,unsigned curIndex,Vec2D &intsecPos)
{
    unsigned index;
    m_contour1.clear();
    m_contour2.clear();
    if(result == 1)//head.
    {
        m_contour1.vertList.push_back(m_linelist[m_cursor].getHead());
        for(index = m_cursor; index != curIndex;)
        {
            m_contour1.vertList.push_back(m_linelist[index].getTail());
            index = m_lop.loopAdd(index,1);
        }
        m_contour1.vertList.push_back(m_linelist[m_cursor].getHead());

        m_contour2.vertList.push_back(m_linelist[curIndex].getHead());
        for(index = curIndex; index != m_cursor;)
        {
            m_contour2.vertList.push_back(m_linelist[index].getTail());
            index = m_lop.loopAdd(index,1);
        }
        m_contour2.vertList.push_back(m_linelist[curIndex].getHead());
        }
   else if(result == 2)//tail.
   {
        m_contour1.vertList.push_back(m_linelist[m_cursor].getHead());
        for(index = m_cursor; index != curIndex;)
        {
         m_contour1.vertList.push_back(m_linelist[index].getTail());
         index = m_lop.loopAdd(index,1);
        }
        m_contour1.vertList.push_back(m_linelist[curIndex].getTail());
        m_contour1.vertList.push_back(m_linelist[m_cursor].getHead());

        for(index = curIndex; index != m_cursor;)
        {
         m_contour2.vertList.push_back(m_linelist[index].getTail());
         index = m_lop.loopAdd(index,1);
        }
        m_contour2.vertList.push_back(m_linelist[curIndex].getTail());
   }
   else //middle.
   {
        m_contour1.vertList.push_back(m_linelist[m_cursor].getHead());
        for(index = m_cursor; index != curIndex;)
        {
            m_contour1.vertList.push_back(m_linelist[index].getTail());
            index = m_lop.loopAdd(index,1);
        }
        m_contour1.vertList.push_back(intsecPos);
        m_contour1.vertList.push_back(m_linelist[m_cursor].getHead());

        m_contour2.vertList.push_back(intsecPos);
        for(index = curIndex; index != m_cursor;)
        {
            m_contour2.vertList.push_back(m_linelist[index].getTail());
            index = m_lop.loopAdd(index,1);
        }
        m_contour2.vertList.push_back(intsecPos);
   }
}

bool HandelBallOutline::getSelfCutPoint(ERItem &item)
{
    ERList::iterator erit;
    unsigned curIndex;
    int endtype;
    Vec2D baseVec,basePos,vec,intsecPos;

    for(erit = item.erlist.begin(); erit != item.erlist.end(); erit++)
    {
        endtype = erit->getEndType();
        curIndex = erit->getIndex();
        intsecPos = erit->getInterSecPos();

        baseVec = m_linelist[m_cursor].getVector();
        baseVec = baseVec.VerticalXYLeft();
        basePos = m_linelist[m_cursor].getHead();

        switch(endtype)
        {
            case 1:
                intsecPos = m_linelist[curIndex].getHead();
                break;
            case 2:
                intsecPos = m_linelist[curIndex].getTail();
                break;
            default:
                break;
        }

        vec = intsecPos - basePos;
        vec.Normalize();

        //in case of ][
        if(baseVec.Dot(vec) > 0)
        {
            unlashSelfLoop(endtype,curIndex,intsecPos);

            //rp_data->m_simuBall->addContour(m_contour1,m_curZ);
            //rp_data->m_simuBall->addContour(m_contour2,m_curZ);

            return true;
        }
      }//for

    return false;
}

bool HandelBallOutline::isIntsectOther(RP2DDisk &ball)
{
    RPboundaries::iterator listIt;//nextIt;
    RPlines2D linelist;

    linelist.clear();
    unsigned linesize = 0;
    unsigned index = 0;


    for(listIt = m_boundrefer.begin(); listIt !=  m_boundrefer.end(); listIt++)
    {
       if(listIt->isJump())
            continue;

        std::copy(listIt->lineList.begin(),listIt->lineList.end(),std::back_inserter(linelist));
        linesize = linelist.size();

       for(index = 0; index < linesize; index++)
       {
            if(isInterOtherOutline(ball,linelist, index))
            {
                listIt->switchJump(true);
                return true;
            }

       }//for linelist
    }//for boundary

    return false;
}

bool HandelBallOutline::isInterOtherOutline(RP2DDisk &ball,RPlines2D &linelist,unsigned hitIndex)
{
    int endname;
    Vec2D intsecPos;

    if(ball.isIntersect(linelist[hitIndex],endname,intsecPos))
    {
        unlashOtherLoop(linelist,endname,hitIndex,intsecPos);

        rp_data->m_simuBall->addContour(m_contour1,m_curZ);

        return true;
    }

    return false;
}

void HandelBallOutline::unlashOtherLoop(RPlines2D &linelist,int result,unsigned hitIndex,Vec2D &intsecPos)
{
    unsigned index;
    m_contour1.clear();

    LoopOperate other_lop = LoopOperate(linelist.size());
    m_contour1.vertList.push_back(m_linelist[m_cursor].getHead());

    for(index = m_cursor; index != (unsigned)m_lop.loopMinus(m_cursor,1);index = (unsigned)m_lop.loopAdd(index,1))
    {
        m_contour1.vertList.push_back(m_linelist[index].getTail());
    }
    m_contour1.vertList.push_back(m_linelist[m_cursor].getHead());

    if(result == 1)//head.
    {
        m_contour1.vertList.push_back(linelist[hitIndex].getHead());
        for(index = hitIndex; index != (unsigned)other_lop.loopMinus(hitIndex,1);index = (unsigned)other_lop.loopAdd(index,1))
        {
            m_contour1.vertList.push_back(linelist[index].getTail());
        }
        m_contour1.vertList.push_back(linelist[hitIndex].getHead());
    }
    else if(result == 2)//tail.
    {
        for(index = hitIndex; index != (unsigned)other_lop.loopMinus(hitIndex,1);index = (unsigned)other_lop.loopAdd(index,1))
        {
            m_contour1.vertList.push_back(linelist[index].getTail());
        }
        m_contour1.vertList.push_back(linelist[hitIndex].getTail());
    }
    else  //intersection point.
    {
        m_contour1.vertList.push_back(intsecPos);
        for(index = hitIndex; index != (unsigned)other_lop.loopMinus(hitIndex,1);index = (unsigned)other_lop.loopAdd(index,1))
        {
            m_contour1.vertList.push_back(linelist[index].getTail());
        }
        m_contour1.vertList.push_back(linelist[hitIndex].getHead());
        m_contour1.vertList.push_back(intsecPos);
    }
     m_contour1.vertList.push_back(m_linelist[m_cursor].getHead());
}

int HandelBallOutline::isIntersectLine(RPline2D &line1, RPline2D &line2, Vec2D &spoint)
{
    spoint = line1.intersect(line2);
    int status = 0;

    if(line2.isPointOnLine(spoint,status))
    {
        return status;
    }
    return status;
}
