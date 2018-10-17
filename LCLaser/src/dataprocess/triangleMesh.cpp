/***************************************************************************
                            triangleMesh.cpp  -  description
                             -------------------
    begin                : Jun. 5 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
    descrition           : providing base polygon elements
 ***************************************************************************/
#include "triangleMesh.h"
#include <iostream>
#include <vector>
#include <fstream>

using namespace DataRP;

TriangleMesh::TriangleMesh()
{
}

TriangleMesh::TriangleMesh(const PolygonMesh& source):PolygonMesh(source)
{
}

TriangleMesh::~TriangleMesh()
{
}

TriangleMesh::FaceIterator TriangleMesh::addFace(const std::vector<TriangleMesh::VertexIterator>& vertices,TriangleMesh::EdgeHasher* edgeHasher)
    {
    /* Add the face directly if it is a triangle; otherwise, triangulate it first: */
    if(vertices.size()==3)
        return PolygonMesh::addFace(vertices,edgeHasher);
    else
        {
        FaceIterator faceIt;
        VertexIterator triangleVertices[3];
        triangleVertices[0]=vertices[0];
        for(size_t i=2;i<vertices.size();++i)
            {
            triangleVertices[1]=vertices[i-1];
            triangleVertices[2]=vertices[i];
            faceIt= PolygonMesh::addFace(3,triangleVertices,edgeHasher);
            }

        return faceIt;
        }
    }

TriangleMesh::FaceIterator TriangleMesh::addFace(int numVertices,const TriangleMesh::VertexIterator vertices[],TriangleMesh::EdgeHasher* edgeHasher)
    {
    /* Add the face directly if it is a triangle; otherwise, triangulate it first: */
    if(numVertices==3)
        return PolygonMesh::addFace(numVertices,vertices,edgeHasher);
    else
        {
        FaceIterator faceIt;
        VertexIterator triangleVertices[3];
        triangleVertices[0]=vertices[0];
        for(int i=2;i<numVertices;++i)
            {
            triangleVertices[1]=vertices[i-1];
            triangleVertices[2]=vertices[i];
            faceIt=PolygonMesh::addFace(3,triangleVertices,edgeHasher);
            }

        return faceIt;
        }
    }


void TriangleMesh::triangulateAllFaces(void)
{
    /* Validate all vertices: */
    ++version;

    /* Find all non-triangular faces and triangulate them: */
    for(FaceIterator faceIt=beginFaces();faceIt!=endFaces();++faceIt)
        {
        /* Triangulate the face if it is not a triangle: */
        if(faceIt->getNumEdges()>3)
            triangulateFace(faceIt);
        }

    /* Re-calculate vertex normal vectors: */
    updateVertexNormals();
}

void TriangleMesh::setFaceTravedtype(short travetype)
{
    short type;
    if(travetype == TRAVE_EVEN)
    {
        type = TRAVE_ODD;
    }
    else if(travetype == TRAVE_ODD)
    {
        type = TRAVE_EVEN;
    }

    for(PolygonMesh::FaceIterator fIt= beginFaces(); fIt!= endFaces(); ++fIt)
    {
        fIt->setTraveType(type);
    }

}

//slice.
void TriangleMesh::cutByPlane(Plane splane, VerticesGrp &boundaries, short travetype)
{
    double minZ, maxZ;
    double curZ = splane.getPoint().getZ();
    Vec2D rangeZ;
    m_travetype = travetype;

    for(PolygonMesh::FaceIterator fIt= beginFaces(); fIt!= endFaces(); ++fIt)
    {
        //already checked.
        if(fIt->getTraveType() == m_travetype)
        {
            continue;
        }
        rangeZ = fIt->getRangeZ();
        minZ = rangeZ.getX(); maxZ = rangeZ.getY();
        //out of the range.
        if(curZ < minZ || curZ > maxZ)
        {
            fIt->setTraveType(m_travetype);
            continue;
        }
        //mark face start face
        m_startFaceIt = fIt;
        m_loop.clear();

        if(cutFirstFace(splane))
        {
           if( *m_loop.begin() == *(m_loop.end()-1) && m_loop.size() > 3  )//must be a loop
               boundaries.push_back(m_loop);
        }
    }
}

bool TriangleMesh::cutFirstFace(Plane splane)
{
    m_startFaceIt->setTraveType(m_travetype);
    vector<FaceEdgeIterator> cutEdgeIts;
    RPVertices interPoints;
    FaceEdgeIterator feIt,targetFaceEdgeIt,cutIt,lineNextIt, linePreIt;  
    Vec3D faceNormal,point,crossPoint;
    Line cutLine;
    int index = 0;
    unsigned overlapType = 0;
    bool b_reverseDirection;

     for(feIt = m_startFaceIt.beginEdges(); feIt != m_startFaceIt.endEdges();++feIt)
     {
         feIt->setHeadOnPlane(false);
     }

    //search number of points on line.
    for(feIt = m_startFaceIt.beginEdges(); feIt != m_startFaceIt.endEdges();++feIt)
    {
        point = *(feIt->getStart());
        if(splane.isOnPlane(point))
        {
            feIt->setHeadOnPlane(true);
            overlapType++;
        }
    }

    faceNormal = m_startFaceIt->getNormal();

    switch(overlapType)
    {
        case 1://because cannot determin search direction.
        case 3:
            return false;
        break;
        case 2:
            for(feIt = m_startFaceIt.beginEdges(); feIt != m_startFaceIt.endEdges();++feIt)
            {

                if(feIt.advanceFace() == m_startFaceIt.endEdges())
                {
                    lineNextIt  = m_startFaceIt.beginEdges();
                }
                else
                {
                     lineNextIt = feIt.advanceFace();
                }

                linePreIt = feIt->getFacePred();

                if(feIt->isHeadOnPlane() && lineNextIt->isHeadOnPlane())
                {
                    cutIt = linePreIt;
                    cutEdgeIts.push_back(cutIt);

                    cutIt = lineNextIt;
                    cutEdgeIts.push_back(cutIt);

                    point = *(feIt->getStart());
                    interPoints.push_back(point);

                    point = *(lineNextIt->getStart());
                    interPoints.push_back(point);
                    break;
                }
            }
        break;
        default:  //0
            index = 0;
            for(feIt = m_startFaceIt.beginEdges(); feIt != m_startFaceIt.endEdges();++feIt)
            {
                Vec3D vhead = *(feIt->getStart());
                Vec3D vtail = *(feIt->getEnd());

                if(splane.intersect(vhead,vtail,crossPoint))
                {
                    interPoints.push_back(crossPoint);
                    cutIt = feIt;
                    cutEdgeIts.push_back(cutIt);
                    index++;
                }
            }//for
            if(index!=2)
            {
                cout<<"intersect points size is not 2"<<endl;
            }
        break;
    }//switch

    if(interPoints.size() ==0 )
        return false;

    cutLine = Line(interPoints[0],interPoints[1]);

    b_reverseDirection = reverseLineDirection(cutLine,faceNormal);

    if(b_reverseDirection)//reversed
    {
        targetFaceEdgeIt = cutEdgeIts[0];
    }
    else   //not reverse.
    {
        targetFaceEdgeIt = cutEdgeIts[1];
    }

    m_loop.push_back(cutLine.getHead());
    m_loop.push_back(cutLine.getTail());

    FaceIterator faceIt;
    if(targetFaceEdgeIt.getCurrent() == NULL)
        return false;

    if(targetFaceEdgeIt->getOpposite() == NULL)
        return false;

    m_NextEdgeIt = EdgeIterator(targetFaceEdgeIt->getOpposite());
    faceIt = FaceIterator(m_NextEdgeIt->getFace());
    if(faceIt == m_startFaceIt || faceIt == NULL)
        return false;

    int result = cutNextFace(splane);

    while( result == 0)
    {
        result = cutNextFace(splane);
    }

    if(result == -1)
        return false;

    return true;
}

//!face with the save edge.
int TriangleMesh::cutNextFace(Plane splane)
{
    FaceIterator faceIt = FaceIterator(m_NextEdgeIt->getFace());
    faceIt->setTraveType(m_travetype);

    FaceEdgeIterator targetFaceEdgeIt,feIt,lineNextIt;
    Vec3D interPoint,point,crossPoint;

    int index = 0;
    unsigned overlapType = 0;

    for(feIt = faceIt.beginEdges(); feIt != faceIt.endEdges();++feIt)
    {
        feIt->setHeadOnPlane(false);
    }

    //search number of points on line.
    for(feIt = faceIt.beginEdges(); feIt != faceIt.endEdges();++feIt)
    {
       point = *(feIt->getStart());
       if(splane.isOnPlane(point))
       {
           feIt->setHeadOnPlane(true);
           overlapType++;
       }
    }

    switch(overlapType)
    {
        case 3:
            m_loop.clear();
            return -1;
        break;
        case 2:
            for( feIt = faceIt.beginEdges(); feIt != faceIt.endEdges();++feIt )
            {

                if( feIt.advanceFace() == m_startFaceIt.endEdges() )
                {
                    lineNextIt  = m_startFaceIt.beginEdges();
                }
                else
                {
                    lineNextIt = feIt.advanceFace();
                }

                if( feIt->isHeadOnPlane() && lineNextIt == m_NextEdgeIt )
                {
                    interPoint = *(feIt->getStart());
                    targetFaceEdgeIt = feIt;
                    break;
                }// on plane.
            }//for
        break;
        default:  //0,1
            index = 0;
            for( feIt = faceIt.beginEdges(); feIt != faceIt.endEdges();++feIt )
            {
                if( feIt != m_NextEdgeIt )
                {
                    Vec3D vhead = *(feIt->getStart());
                    Vec3D vtail = *(feIt->getEnd());

                    if(splane.intersect(vhead,vtail,crossPoint))
                    {
                        interPoint = crossPoint;
                        targetFaceEdgeIt = feIt;
                        index++;
                    }
                }
            }//for
            if(index > 1)
            {
                qDebug()<<"next face intersect points size is large than 1"<<endl;
                return -1;
            }
            if(index ==0 )
            {
                qDebug()<<"erro no line cut"<<endl;
                return -1;
            }
       break;
    }//switch

    point = *(m_loop.end()-1);
    if(!point.IsApproximateEqual(interPoint,1))
        m_loop.push_back(interPoint);

    if(targetFaceEdgeIt.getCurrent() == NULL)
        return -1;

    if(targetFaceEdgeIt->getOpposite() == NULL)
        return -1;

    m_NextEdgeIt = EdgeIterator(targetFaceEdgeIt->getOpposite());
    faceIt = FaceIterator(m_NextEdgeIt->getFace());
    if(faceIt == m_startFaceIt || faceIt == NULL)
    {
        m_loop.pop_back();
        m_loop.push_back(*m_loop.begin());
        return 1;
    }

   return 0;
}


TriangleMesh::VertexHasher* TriangleMesh::startAddingVertex()
{
    return new VertexHasher(101);
}

void TriangleMesh::finishAddingVertices(TriangleMesh::VertexHasher* vertexHasher)
{
    delete vertexHasher;
}


void TriangleMesh::debugShowAll()
{
    Vec3D point;
    ofstream out("out.txt");
    if (!out.is_open())
        return;

    out<<"solid CATIA STL"<<endl;
    for(ConstFaceIterator fIt = this->beginFaces(); fIt != this->endFaces(); ++fIt)
    {
        out<<"  facet normal "<<"0 0 0"<<endl;
        out<<"    outer loop"<<endl;
        for(ConstFaceEdgeIterator feIt = fIt.beginEdges(); feIt != fIt.endEdges(); ++feIt)
        {
            point = (*feIt->getStart());
            out<<"        vertex "<<point.getX()<<" "<<point.getY()<<" "<<point.getZ()<<endl;
        }
        out<<"    endloop"<<endl;
        out<<"  endfacet"<<endl;
    }
    out<<"endsolid CATIA STL"<<endl;
    out.close();
}

bool TriangleMesh::reverseLineDirection(Line &line, Vec3D normal)
{
    Vec3D tnormal = normal;
    tnormal.setZ(0);

    Vec3D lineNormal = line.getVector();
    Vec3D direction = lineNormal.VerticalXYRight();
    if(tnormal.Dot(direction)> 0)
    {
        return false;
    }

    line.reverseLine();
    return true;
}
