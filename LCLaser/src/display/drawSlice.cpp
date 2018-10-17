/***************************************************************************
                             drawSlice.cpp  -  description
                             -------------------
    begin                : Jun. 3 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "drawSlice.h"

using namespace RenderRP;

RPDrawSlice::RPDrawSlice()
{
}

RPDrawSlice::RPDrawSlice(DataRP::RPData *rpdata):RPdraw(rpdata)
{
}

RPDrawSlice::~RPDrawSlice()
{

}

void RPDrawSlice::draw()
{
     DataRP::RPslices::iterator itsli;

    int currentlayer = rp_data->getCurrentLayer();

    if(rp_data->isShowAll())
    {
       for(itsli = rp_data->m_SlcSlices.begin(); itsli != rp_data->m_SlcSlices.end(); itsli++ )
       {
           drawBoundaries(*itsli);
       }//slices
    }
    else
    {
       drawBoundaries(rp_data->m_SlcSlices.at((unsigned)currentlayer));
    }

    drawPaths(currentlayer);

    if(rp_data->isShowCut())
       drawCutPlane();

}

void RPDrawSlice::drawCutPlane()
{
    Vec3D wing[2];
    DataRP::RPVertices normaLine;
    Vec3D  planepoint = rp_data->m_cutPlane.getPoint();
    Vec3D  normal = rp_data->m_cutPlane.getNormal();
    double length = rp_data->m_boundingBox.getMaxDownEdgeXY();

    Vec3D normalend = planepoint + normal*length/2;

    normaLine.push_back(planepoint);
    normaLine.push_back(normalend);

    normal.VerticalXY(wing[0],wing[1]);

    Vec3D lw = planepoint + wing[0]*length/2;
    Vec3D rw = planepoint + wing[1]*length/2;

    glBegin(GL_LINES);
      glColor3f(0.0f,0.0f,1.0f);  //cut plane color

      glVertex3d(lw.getX(),lw.getY(),lw.getZ());
      glVertex3d(rw.getX(),rw.getY(),rw.getZ());

      glVertex3d(planepoint.getX(),planepoint.getY(),planepoint.getZ());
      glVertex3d(normalend.getX(),normalend.getY(),normalend.getZ());
   glEnd();

      Graphs::drawArrows(normaLine,planepoint.getZ(),m_scale,true);

}


void RPDrawSlice::drawBoundaries(DataRP::RPslice slice)
{
    DataRP::RPVertices lines;
    DataRP::RPboundaries::iterator itbond;
    DataRP::RPpolygon::iterator itver;
    glLineWidth(1);

    for(itbond = slice.outerBoundaries.begin(); itbond !=  slice.outerBoundaries.end(); itbond++)
    {
      lines.clear();
      glColor3f(1.0f,0.0f,0.0f); //slc color
      glBegin(GL_LINE_STRIP);

      for(itver = itbond->vertList.begin(); itver != itbond->vertList.end(); itver++)
      {
         glVertex3d(itver->x,itver->y,slice.minZLevel);
         //qDebug()<<"coord "<<itver->x<<" "<<itver->y<<" "<<slice.minZLevel<<endl;
         lines.push_back(Vec3D(itver->x,itver->y,0));
      }//vertex

      glEnd();

     //draw arrows
      if(!rp_data->isShowAll()&&b_arrow)
      {
         Graphs::drawArrows(lines,slice.minZLevel,m_scale,true);
      }

    }//outerBoundaries
      // draw bounding box.

    if(b_aabb)
        drawAABB();

    glLineWidth(1);
}

void RPDrawSlice::drawAABB()
{
    unsigned numbpoints;

    DataRP::RPVertices vers;
    vers.push_back(rp_data->m_boundingBox.peaks[0]); vers.push_back(rp_data->m_boundingBox.peaks[4]);
    vers.push_back(rp_data->m_boundingBox.peaks[1]); vers.push_back(rp_data->m_boundingBox.peaks[5]);
    vers.push_back(rp_data->m_boundingBox.peaks[2]); vers.push_back(rp_data->m_boundingBox.peaks[6]);
    vers.push_back(rp_data->m_boundingBox.peaks[3]); vers.push_back(rp_data->m_boundingBox.peaks[7]);

    glColor3f(0.0f,1.0f,0.0f);

    numbpoints = 8;

    glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3,GL_DOUBLE,0,vers.data());
      glDrawArrays(GL_LINES,0,numbpoints);

    numbpoints = 4;

      glVertexPointer(3,GL_DOUBLE,0,rp_data->m_boundingBox.peaks);
      glDrawArrays(GL_LINE_LOOP,0,numbpoints);

      glVertexPointer(3,GL_DOUBLE,0,rp_data->m_boundingBox.peaks);
      glDrawArrays(GL_LINE_LOOP,4,numbpoints);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void RPDrawSlice::drawPaths(int currentlayer)
{
    if(rp_data->m_SlcSlices.at((unsigned)currentlayer).outlines.size() > 0)
          drawPathOutline();

    if(rp_data->m_SlcSlices.at((unsigned)currentlayer).paths.size() > 0)
          drawPathParallel();
}

void RPDrawSlice::drawPathOutline()
{
    DataRP::RPboundaryGroup::iterator itgp,lastgp;
    DataRP::RPboundaries::iterator itbond;
    DataRP::RPpolygon::iterator itver;
    DataRP::RPVertices lines;
    unsigned curlayer = (unsigned)rp_data->getCurrentLayer();

    if(rp_data->isDrawProcess())
        lastgp = rp_data->m_SlcSlices.at(curlayer).outlines.end()-1;
    else
        lastgp = rp_data->m_SlcSlices.at(curlayer).outlines.end();


    for(itgp = rp_data->m_SlcSlices.at(curlayer).outlines.begin();itgp != lastgp; itgp++)
    {
        for(itbond = itgp->begin(); itbond != itgp->end(); itbond++)
        {
            lines.clear();
            for(itver = itbond->vertList.begin(); itver != itbond->vertList.end(); itver++)
            {
                lines.push_back(Vec3D(itver->x,itver->y,rp_data->m_SlcSlices.at(curlayer).minZLevel));
            }
            Graphs::drawLines(lines,1,DataRP::RPColor(0.0,1.0,0.0),true);

            if(b_arrow)
            {
                Graphs::drawArrows(lines,m_scale,true,true);
            }
        }//loop outlines.
    }//loop outline groups.
}

void RPDrawSlice::drawPathParallel()
{
    DataRP::RPlines2D::iterator itpath;
    DataRP::RPVertices lines;
    unsigned curlayer = (unsigned)rp_data->getCurrentLayer();

    for(itpath = rp_data->m_SlcSlices.at(curlayer).paths.begin(); itpath != rp_data->m_SlcSlices.at(curlayer).paths.end();itpath++)
    {
        lines.push_back(Vec3D(itpath->lhead.getX(),itpath->lhead.getY(),rp_data->m_SlcSlices.at(curlayer).minZLevel));
        lines.push_back(Vec3D(itpath->ltail.getX(),itpath->ltail.getY(),rp_data->m_SlcSlices.at(curlayer).minZLevel));
    }

    Graphs::drawLines(lines,1,DataRP::RPColor(0.0,1.0,0.0));
    Graphs::drawArrows(lines,m_scale);

}


