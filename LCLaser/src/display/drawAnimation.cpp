/***************************************************************************
                             drawAnimation.cpp  -  description
                             -------------------
    begin                : Jun. 11 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "drawAnimation.h"

using namespace RenderRP;

RPDrawAnimation::RPDrawAnimation()
{
}

RPDrawAnimation::RPDrawAnimation(DataRP::RPData *rpData):RPdraw(rpData)
{
}

RPDrawAnimation::~RPDrawAnimation()
{

}

void RPDrawAnimation::draw()
{
    drawLayerTracks();
    drawAnimTrack();
    drawAnimArc();
    if(b_param)
        drawLaserHead();
}

void RPDrawAnimation::drawLaserHead()
{
    Vec3D pos = rp_data->getLaserHeadPosition();
    glTranslatef(pos.getX(),pos.getY(),pos.getZ());
       RenderRP::Graphs::modelLaserHead(60.0, 15.0, 3.0, 12, rp_data->isLaserOn());
    glTranslatef(-pos.getX(),-pos.getY(),-pos.getZ());
}

void RPDrawAnimation::drawLayerTracks()
{
    drawTracks(rp_data->m_drawTracks);

    DataRP::LayerTracks::iterator lit;
    for(lit = rp_data->m_layerTracks.begin(); lit != rp_data->m_layerTracks.end(); lit++)
    {
        drawTracks(*lit);
    }
}

void RPDrawAnimation::drawTracks(DataRP::DrawTracks &drawTracks)
{

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 1.0f, 1.0f, 0.5f);

    if(drawTracks.size() > 0)
    {
        int trackType;
        Vec3D va,vb,vc,vd;
        double radius,radian;
        short dir;
        double trackWidth;
        DataRP::DrawTracks::iterator vit;
        for(vit =drawTracks.begin();vit != drawTracks.end();vit++)
        {
            trackType = (*vit)->getType();
            trackWidth = (*vit)->getTrackWidth();
             (*vit)->getParam(va,vb,vc,vd,radius,radian,dir);

            switch(trackType)
            {
                case DataRP::TRACK_TERMINAL:
                   Graphs::drawTrackTerminal(va,vb,trackWidth,12);
                break;

                case DataRP::TRACK_LINE:
                    Graphs::drawTrackBody(va,vb,vc,vd);
                break;

                case DataRP::TRACK_ELBOW:
                    Graphs::drawElbow(va,vb,vc,vd,trackWidth,dir);
                break;

                case DataRP::TRACK_ARC:
                    Graphs::drawArc(va,vc,vb,radian,radius,trackWidth);
                break;

                default:
                break;
            }
        }//for
    }


    glDisable(GL_BLEND);
}

void RPDrawAnimation::drawAnimTrack()
{
    if(!(rp_data->isLaserOn()&& rp_data->isTrackAnimate()))
    {
        return;
    }

    double radius = rp_data->getCompenRadius();
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
    Vec3D tstart,tend;
    rp_data->getAnimateTrack(tstart,tend);
    RenderRP::Graphs::modleTrack(tstart,tend,radius);
}

void RPDrawAnimation::drawAnimArc()
{
 if(!(rp_data->isLaserOn()&& rp_data->isArcAnimate()))
    {
        return;
    }
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
    Vec3D start,center;
    double radian, radius,trackWidth;
    rp_data->getAnimateArc(start,center,radian,radius,trackWidth);
    RenderRP::Graphs::drawAnimArc(start,center,radian,radius,trackWidth);
}





