/***************************************************************************
                          graph.h  -  description
                             -------------------
    begin                : May 2 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef GRAPH_H
#define GRAPH_H

#include <math.h>
#include "displaybase.h"
#include "database.h"

namespace RenderRP
{

    class Graphs
    {

    public:
        static void modelLaserHead(double height,double coneface,double width,double slices,bool isopen);
        static void modleTrack(Vec3D tstart,Vec3D tend,double radius);
        static void FilledArc(Vec3D center,Vec3D arm,double radius, double slices);
        static void modelCube();
        static void drawGrid(Grid &grid);

        static void drawLines(DataRP::RPVertices &lines, float linewidth, DataRP::RPColor color,bool bstrip = false);
        static void drawLines(DataRP::Tracks &lines,float linewidth, DataRP::RPColor color,float scale, bool showArrow = false);

        static void drawArrows(DataRP::RPVertices &lines,float scale, bool bstrip = false, bool onhead = false);
        static void drawArrows(DataRP::RPVertices &lines,double vz,float scale,bool onhead = false);
        static void drawArrow(const Line &line,DataRP::RPColor color);

        static void drawTrackHead(Line &line,double radius);
        static void drawTrackEnd(Line &line,double radius);
        static void drawTrackTerminal(Vec3D scenter,Vec3D svec, double sradius,double slices = 12);
        static void drawTrackBody(Vec3D leftstart,Vec3D rightstart,Vec3D leftend,Vec3D rightend);
        static void drawElbow(Vec3D start,Vec3D send,Vec3D interPoint,Vec3D center, double sradius, short dir);

        static void drawArc(Vec3D start,Vec3D center, Vec3D send,double radian,double sradius,double trackWidth);
        static void drawAnimArc(Vec3D start,Vec3D center, double radian, double sradius,double trackWidth);

        static void drawDisk(const Vec3D &center,double radius, double slices,DataRP::RPColor color);

        static void draw2DFrameBar(DataRP::RPpolygon &vertices);
        static void draw2DBar(DataRP::RPpolygon &vertices);
    };
}
#endif // GRAPH_H
