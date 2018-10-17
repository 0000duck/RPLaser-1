/***************************************************************************
                          dataDrwTrack.cpp  -  description
                             -------------------
    begin                : Aug. 26 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "dataDrawTrack.h"

using namespace DataRP;

DataDrawTrack::DataDrawTrack()
{
}

DataDrawTrack::~DataDrawTrack()
{
}

DataDrawTrack::DataDrawTrack(int stype):
               m_type(stype)
{
}

DataDrawTrack::DataDrawTrack(int stype,double sradius):
               m_type(stype),m_trackWidth(sradius)
{
}

int DataDrawTrack::getType()
{
    return m_type;
}

double DataDrawTrack::getTrackWidth()
{
    return m_trackWidth;
}

TrackTerminal::TrackTerminal(Vec3D scenter,Vec3D svec, double trackWidth):
               DataDrawTrack(TRACK_TERMINAL,trackWidth)
{
    m_center = scenter;
    m_vertVec = svec;
}

void TrackTerminal::getParam(Vec3D &va,Vec3D &vb,Vec3D &vc,Vec3D &vd,double &radius, double &radian, short &dir)
{
    va = m_center;
    vb = m_vertVec;
    vc = Vec3D(0.0,0.0,0.0);
    vd = Vec3D(0.0,0.0,0.0);
    radius = 0;
    radian = 0;
    dir = 0;
}


TrackLine::TrackLine(Vec3D leftstart,Vec3D rightstart,Vec3D leftend,Vec3D rightend):
               DataDrawTrack(TRACK_LINE)
{
    m_leftStart = leftstart;
    m_rightStart= rightstart;
    m_leftEnd = leftend;
    m_righEnd = rightend;
}

void TrackLine::getParam(Vec3D &va,Vec3D &vb,Vec3D &vc,Vec3D &vd,double &radius, double &radian, short &dir)
{
    va = m_leftStart;
    vb = m_rightStart;
    vc = m_leftEnd;
    vd = m_righEnd;
    radius = 0;
    radian = 0;
    dir = 0;
}


TrackElbow::TrackElbow(Vec3D start,Vec3D send,Vec3D interPoint,Vec3D center, double trackWidth,short direction):
               DataDrawTrack(TRACK_ELBOW,trackWidth)
{
    m_start = start;
    m_end = send;
    m_interpos = interPoint;
    m_center = center;
    m_dir = direction;
 }


void TrackElbow::getParam(Vec3D &va,Vec3D &vb,Vec3D &vc,Vec3D &vd,double &radius, double &radian, short &dir)
{
    va = m_start;
    vb = m_end;
    vc = m_interpos;
    vd = m_center;
    radius = 0;
    radian = 0;
    dir = m_dir;
}

TrackArc::TrackArc(Vec3D start,Vec3D send,Vec3D center, double sradius, double radian, double trackWidth):
               DataDrawTrack(TRACK_ARC,trackWidth)
{
    m_start = start;
    m_end = send;
    m_center = center;
    m_radius = sradius;
    m_radian = radian;
 }


void TrackArc::getParam(Vec3D &va,Vec3D &vb,Vec3D &vc,Vec3D &vd,double &radius, double &radian, short &dir)
{
    va = m_start;
    vb = m_end;
    vc = m_center;
    vd = Vec3D(0.0,0.0,0.0);
    radius = m_radius;
    radian = m_radian;
    dir = 0;
}

