/***************************************************************************
                          dataSlice.h  -  description
                             -------------------
    begin                : Apr. 22 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef DATASLICE_H
#define DATASLICE_H

#include "database.h"

namespace DataRP
{
/**************Slices  Data******************/
    struct RPcontour
    {
       RPpolygon vertList;
       RPlines2D lineList;
       RPPrintTrack printTrack;
       unsigned numb_verts;
       unsigned numb_gaps;
       bool bjump;

       RPcontour():bjump(false)
       {}

       void clear()
       {
           vertList.clear();
           lineList.clear();
           printTrack.clear();
           bjump = false;
       }

       void calnumVerts()
       {
            numb_verts = vertList.size();
            genLineList();
            numb_gaps = 0;
       }

       void genLineList()
       {
            lineList.clear();
            RPpolygon::iterator itver;
            Vec2D lhead,ltail;
            for(itver = vertList.begin()+1; itver != vertList.end(); itver++)
            {
                lhead = *(itver-1);
                ltail = *itver;
                lineList.push_back(RPline2D(lhead,ltail));
            }
       }

       void switchJump(bool jump)
       {
            bjump = jump;
       }

       bool isJump()
       {
            return bjump;
       }
    };

      typedef vector<RPcontour> RPboundaries;
      typedef list<RPcontour> RPboundlist;

      typedef vector<RPboundaries> RPboundaryGroup;

    struct RPslice
    {
        RPboundaries outerBoundaries;
        RPboundaryGroup outlines;
        RPlines2D paths;
        double minZLevel;
        unsigned numb_outerBoundaries;
        bool b_pathOk;

        void clear()
        {
            outerBoundaries.clear();
            outlines.clear();
            paths.clear();
            b_pathOk = false;
        }

        double getZ()
        {
            return minZLevel;
        }

        //!for the command thread. if not OK, Waiting.
        bool isPathOK()
        {
            return b_pathOk;
        }

        void setPathOk()
        {
            b_pathOk = true;
        }
    };

    typedef vector<RPslice> RPslices;

    struct HeadSlc
    {
        static QString keywords;

        HeadSlc()
        {
         //keywords = "-SLCVER,-UNIT,-TYPE,-PACKAGE,-EXTENTS";
        }

       struct Extent
       {
         double minx,maxx;
         double miny,maxy;
         double minz,maxz;
         Vec3D center;

         Extent()
         {
             minx = 0;
             maxx = 10;
             miny = 0;
             maxy = 10;
             minz = 0;
             maxz = 10;
             center = Vec3D(5.0,5.0,0.0);
         }

         Extent(double minX,double maxX,double minY,double maxY, double minZ, double maxZ )
         {
             minx = minX;
             maxx = maxX;
             miny = minY;
             maxy = maxY;
             minz = minZ;
             maxz = maxZ;
             setCenter();
         }

         void setCenter()
         {
            double x,y,z;
            x = (maxx-minx)/2 + minx;
            y = (maxy-miny)/2 + miny;
            z = (maxz-minz)/2 + minz;
            center = Vec3D(x,y,z);
         }

         Vec3D& getCenter()
         {
            return center;
         }
       };


       float licVer;
       QString unit;
       QString type;
       QString package;
       Extent extents;

       void debug()
       {
           qDebug()<<licVer<<" "<<unit<<" "<<type<<" "<<package<<" "
           <<extents.minx<<" "<<extents.maxx<<" "<<extents.miny<<" "<<extents.maxy<<" "<<extents.minz<<" "<<extents.maxz<<endl;
       }
    };

    struct TableSlc
    {
        float minZLevel;
        float layerThinckness;
        float lineWideCompen;
        float reserved;

        TableSlc(float minz, float layerthinck,float linewidecompen, float reserve )
        {
            minZLevel = minz;
            layerThinckness = layerthinck;
            lineWideCompen = linewidecompen;
            reserved = reserve;
        }

        void debug()
        {
            qDebug()<<"Minimum Z Level: "<<minZLevel<<"Layer Thickness: "<<layerThinckness
                  <<"Line Width Compensation"<<lineWideCompen<<"reserved"<<reserved;

        }
    };

    typedef vector <TableSlc> TableEntries;

}

#endif // DATASLICE_H
