/***************************************************************************
                            triangleMesh.h  -  description
                             -------------------
    begin                : Jun. 5 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
    descrition           : providing base polygon elements
 ***************************************************************************/
#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "polygonMesh.h"

namespace DataRP
{

    class TriangleMesh: public PolygonMesh
    {
    public:
        TriangleMesh();
        TriangleMesh(const PolygonMesh& source);
        ~TriangleMesh();
        FaceIterator addFace(const std::vector<VertexIterator>& vertices,EdgeHasher* edgeHasher);
        FaceIterator addFace(int numVertices,const VertexIterator vertices[],EdgeHasher* edgeHasher);

        void triangulateAllFaces(void);
        void cutByPlane(Plane splane, VerticesGrp &boundaries, short travetype);
        void debugShowAll();

        typedef HashTable<Vec3D,VertexIterator> VertexHasher;
        VertexHasher* startAddingVertex();
        void finishAddingVertices(TriangleMesh::VertexHasher* vertexHasher);

    protected:
        int cutNextFace(Plane splane);
        bool cutFirstFace(Plane splane);
        void setFaceTravedtype(short travetype);
        bool reverseLineDirection(Line &line, Vec3D normal);

    public:
        BoundingBox m_bbox;

    protected:
        FaceIterator m_startFaceIt;
        EdgeIterator m_NextEdgeIt;
        Vec3D m_startVertex;
        RPVertices m_loop;
        short m_travetype;
    };

}

#endif //TRIANGLE_MESH_H
