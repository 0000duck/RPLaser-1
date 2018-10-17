/***************************************************************************
                            polygonMesh.h  -  description
                             -------------------
    begin                : Jun. 5 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
    descrition           : providing base polygon elements
 ***************************************************************************/
#ifndef POLYGON_MESH_H
#define POLYGON_MESH_H

#include <math.h>
#include "displaybase.h"
#include "database.h"
#include "hashtable.h"
#include "matrix4x4.h"

namespace DataRP
{
    enum TraveOrder
    {
        TRAVE_EVEN = 0,
        TRAVE_ODD,
    };

    class PolygonMesh
    {

    public:
        /*embedded classed*/
        class Vertex;
        class Face;

        //! Basic data structure for representing vertices, edges and polygons and their adjacency
        class Edge
        {
        /* Elements: */
        private:
            Vertex* start; //!< Pointer to start vertex of half-edge
            Face* face; //!< Pointer to face
            Edge* facePred; //!< Pointer to next half-edge in clockwise order around a face
            Edge* faceSucc; //!< Pointer to next half-edge in counter-clockwise order around a face
            Edge* opposite; //!< Pointer to opposite half-edge in adjacent polygon
            public:
            int sharpness; //!< Sharpness coefficient of edge for Catmull-Clark subdivision
            Vertex* edgePoint; //!< Pointer to edge point for Catmull-Clark subdivision
            bool b_headonPlane;

            /* Access methods: */
            public:
            //!get start vertex of half-edge.
            const Vertex* getStart() const
            {
                return start;
            }

            //!get start vertex of half-edge.
            Vertex* getStart()
            {
                return start;
            }

             //! Returns end point of half-edge
            const Vertex* getEnd(void) const
            {
                return faceSucc->start;
            }

            //! Returns end point of half-edge
                Vertex* getEnd()
            {
                return faceSucc->start;
            }

             const Face* getFace() const
            {
                return face;
            }

            Face* getFace()
            {
                return face;
            }

            const Edge* getFacePred() const
            {
                return facePred;
            }

            Edge* getFacePred()
            {
                return facePred;
            }

            const Edge* getFaceSucc() const
            {
                return faceSucc;
            }

            Edge* getFaceSucc()
            {
                return faceSucc;
            }

            //! Returns next half-edge around vertex in clockwise order, 0 if doesn't exist
            const Edge* getVertexPred(void) const
            {
                return opposite!=0?opposite->faceSucc:0;
            }

            // Ditto
            Edge* getVertexPred()
            {
                return opposite!=0?opposite->faceSucc:0;
            }

            //! Returns next half-edge around vertex in counter-clockwise order, 0 if doesn't exist

            const Edge* getVertexSucc() const
            {
                return facePred->opposite;
            }

            // Ditto
            Edge* getVertexSucc()
            {
                return facePred->opposite;
            }

            //! Returns next half-edge around edge end vertex in clockwise order, 0 if doesn't exist
            const Edge* getEndVertexPred() const
            {
                return faceSucc->opposite;
            }

            // Ditto
            Edge* getEndVertexPred()
            {
                return faceSucc->opposite;
            }

            //! Returns next half-edge around edge end vertex in counter-clockwise order, 0 if doesn't exist
            const Edge* getEndVertexSucc() const
            {
                return opposite!=0?opposite->facePred:0;
            }

            // Ditto
            Edge* getEndVertexSucc()
            {
                return opposite!=0?opposite->facePred:0;
            }

            const Edge* getOpposite() const
            {
                return opposite;
            }

            Edge* getOpposite()
            {
                return opposite;
            }

            void set(Vertex* sStart,Face* sFace,Edge* sFacePred,Edge* sFaceSucc,Edge* sOpposite)
            {
                start=sStart;
                face=sFace;
                facePred=sFacePred;
                faceSucc=sFaceSucc;
                opposite=sOpposite;
            }

            void setStart(Vertex* sStart)
            {
                start=sStart;
            }

            void setFace(Face* sFace)
            {
                face=sFace;
            }

            void setFacePred(Edge* sFacePred)
            {
                facePred=sFacePred;
            }

            void setFaceSucc(Edge* sFaceSucc)
            {
                faceSucc=sFaceSucc;
            }

            void setOpposite(Edge* sOpposite)
            {
                opposite=sOpposite;
            }

            void setHeadOnPlane(bool bonplane)
            {
              b_headonPlane = bonplane;
            }

            bool isHeadOnPlane()
            {
              return b_headonPlane;
            }
        };

        //! Basic data structure for representing faces
        class Face
        {
        friend class FaceIterator;
        friend class ConstFaceIterator;
        friend class PolygonMesh;

        /* Elements: */
        private:
            Edge* edge; // Pointer to any one of the polygon's edges
            /* Pointers to form a double-linked list: */
            Face* pred;
            Face* succ;
            public:
            Vertex* facePoint; // Pointer to face point for Catmull-Clark subdivision
            mutable bool visited; // Flag to mark faces as visited during triangle strip generation
            Vec2D rangeZ;
            Vec3D normal;
            short traveType;
            /* Constructors and destructors: */
            Face(Face* sSucc)
            :edge(0),pred(0),succ(sSucc),traveType(0)
            {
            }

            /* Methods: */
         public:
            const Edge* getEdge() const
            {
                return edge;
            }

            Edge* getEdge()
            {
                return edge;
            }

            void setEdge(Edge* sEdge)
            {
                edge=sEdge;
            }

            int getNumEdges() const
            {
                int result=0;
                const Edge* ePtr=edge;

                do
                {
                    ++result;
                    ePtr=ePtr->getFaceSucc();
                }
                while(ePtr!=edge);

                return result;
            }

            void setRangeZ(Vec2D &vec)
            {
                rangeZ = vec;
            }

            Vec2D getRangeZ()
            {
                return rangeZ;
            }

            Vec2D getRangeZ() const
            {
                return rangeZ;
            }

            void setNormal(Vec3D snormal)
            {
                normal = snormal;
            }


            Vec3D getNormal()
            {
                return normal;
            }

            Vec3D getNormal() const
            {
                return normal;
            }

            void setTraveType(short stype)
            {
                traveType = stype;
            }

            short getTraveType()
            {
                return traveType;
            }
        };

        class Vertex: public Vec3D
        {
            friend class VertexIterator;
            friend class ConstVertexIterator;
            friend class PolygonMesh;

          public:

            /* Elements: */
            unsigned int version; //!< If this is equal to mesh's version number, vertex was updated
            RPColor color; //!< Vertex color
            RPUV uv;
            Vec3D normal; //!< Vertex normal vector

          private:
            Edge* edge; //!< Pointer to one half-edge starting at the vertex
            Vertex* pred;//!< double link pred.
            Vertex* succ;//!< double link succeed.

            /*Constructors and destructors: */
            Vertex(const Vec3D& sPoint,const RPColor& sColor,const RPUV &suv,Vertex* sSucc)
            :Vec3D(sPoint),color(sColor),uv(suv),edge(0),pred(0),succ(sSucc)
            {
            }

          /* Methods: */
          public:
            //! set vertex's element point.
            void setPoint(const Vec3D& sPoint)
            {
                Vec3D::operator=(sPoint);
            }

            //! get the vertex's edge.
            const Edge* getEdge() const
            {
                return edge;
            }

            //!get the vertex's edge.
            Edge* getEdge()
            {
                return edge;
            }

            //! set the vertex's edge.
            void setEdge(Edge* sEdge)
            {
                edge=sEdge;
            }

            //!get the number of vertex's edges.
            int getNumEdges() const
            {
                int result=0;
                const Edge* ePtr = edge;

                do
                {
                    ++result;
                    ePtr = ePtr->getVertexSucc();
                }
                while(ePtr!=edge);

                return result;
            }

            Vertex* getSucc() //! get succeed point
            {
                return succ;
            }

             //!Checks if the vertex is completely surrounded by faces
            bool isInterior() const
            {
                if(edge==NULL)
                    return false;

                const Edge* ePtr=edge;

                do
                {
                    ePtr=ePtr->getVertexSucc();
                }

                while(ePtr!=edge && ePtr!=NULL);//loop

                return ePtr!=NULL;
            }

            };

        //!Class to calculate affine combinations of mesh vertices
        class VertexCombiner
        {
            /* Elements: */
          private:
            float pointSum[3]; //!< Non-normalized point components
            float colorSum[4]; //!< Non-normalized color components
            float weightSum; //!< Sum of affine weights used in combination

        /* Constructors and destructors: */
        public:
            VertexCombiner(void):weightSum(0.0f)
            {
                for(int i=0;i<3;++i)
                    pointSum[i]=0.0f;

                for(int i=0;i<4;++i)
                    colorSum[i]=0.0f;
            }

        /* Methods: */
            //! Returns the created point
            Vec3D getPoint() const
            {
                Vec3D result = Vec3D(pointSum[0]/weightSum,pointSum[1]/weightSum,pointSum[2]/weightSum);
                return result;
            }

            //! Returns the created color
            RPColor getColor(void) const
            {
                RPColor result;   //floor向下取整
                result = RPColor(floor(colorSum[0]/weightSum+0.5f),floor(colorSum[1]/weightSum+0.5f),floor(colorSum[2]/weightSum+0.5f));
                return result;
            }

            //! Resets the combiner to "empty"
            VertexCombiner& reset()
            {
                for(int i=0;i<3;++i)
                    pointSum[i]=0.0f;

                for(int i=0;i<4;++i)
                    colorSum[i]=0.0f;

                weightSum=0.0f;
                return *this;
            }

             //!Adds vertex with affine weight 1
            VertexCombiner& addVertex(const Vertex* vPtr)
            {
                pointSum[0]+=(vPtr->getX());
                pointSum[1]+=(vPtr->getY());
                pointSum[2]+=(vPtr->getZ());

            for(int i=0;i<4;++i)
                colorSum[i]+=float(vPtr->color.rgba[i]);

                weightSum+=1.0f;
                return *this;
            }

            //! Adds vertex with given affine weight
            VertexCombiner& addVertex(const Vertex* vPtr,float weight)
            {
                    pointSum[0]+=(vPtr->getX())*weight;
                    pointSum[1]+=(vPtr->getY())*weight;
                    pointSum[2]+=(vPtr->getZ())*weight;

                for(int i=0;i<4;++i)
                    colorSum[i]+=float(vPtr->color.rgba[i])*weight;

                weightSum+=weight;
                return *this;
            }
        };

        //! Helper structure for creating polygon meshes
        class VertexPair
        {
            friend class PolygonMesh;

            /* Elements: */
          private:
            const Vertex* vertices[2]; // Pointers to the two vertices (ordered by size_t order)

            /* Constructors and destructors: */
          public:
            VertexPair(void)
            {
            }

            VertexPair(const Vertex* sVertex1,const Vertex* sVertex2)
            {
                if(reinterpret_cast<size_t>(sVertex1)<reinterpret_cast<size_t>(sVertex2))
                {
                    vertices[0]=sVertex1;
                    vertices[1]=sVertex2;
                }
                else
                {
                    vertices[0]=sVertex2;
                    vertices[1]=sVertex1;
               }
            }

            VertexPair(const Edge& edge)
            {
                const Vertex* v1=edge.getStart();
                const Vertex* v2=edge.getEnd();
                if(reinterpret_cast<size_t>(v1)<reinterpret_cast<size_t>(v2))
                {
                    vertices[0]=v1;
                    vertices[1]=v2;
                }
                else
                {
                    vertices[0]=v2;
                    vertices[1]=v1;
                }
            }

            /* Methods: */
            friend bool operator==(const VertexPair& p1,const VertexPair& p2)
            {
                return p1.vertices[0]==p2.vertices[0] && p1.vertices[1]==p2.vertices[1];
            }

            friend bool operator!=(const VertexPair& p1,const VertexPair& p2)
            {
                return p1.vertices[0]!=p2.vertices[0]||p1.vertices[1]!=p2.vertices[1];
            }

            static size_t hash(const VertexPair& p,size_t tableSize)
            {
                size_t val1=reinterpret_cast<size_t>(p.vertices[0]);
                size_t val2=reinterpret_cast<size_t>(p.vertices[1]);
                return (val1*17+val2*31)%tableSize;
            }
        };

        //! Iterator for edges
        class EdgeIterator
        {
            friend class PolygonMesh;
            friend class ConstEdgeIterator;

            /* Elements: */
          private:
            Edge* loopStart; // First edge in loop around vertex/face
            Edge* edge; // Edge pointed to

            /* Constructors and destructors: */
            public:
            EdgeIterator() // Constructs invalid operator
                :loopStart(0),edge(0)
            {
            }

            EdgeIterator(Edge* sEdge)
            :loopStart(sEdge),edge(sEdge)
            {
            }

            /* Methods: */
            friend bool operator==(const EdgeIterator& it1,const EdgeIterator& it2)
            {
            return it1.edge==it2.edge;
            }

            friend bool operator!=(const EdgeIterator& it1,const EdgeIterator& it2)
            {
            return it1.edge!=it2.edge;
            }

            EdgeIterator& operator=(const EdgeIterator &other)
            {
                edge = other.getCurrent();
                loopStart = other.getCurrent();
                return *this;
            }

            Edge* getCurrent() const
            {
                return edge;
            }

            Edge& operator*() const
            {
                return *edge;
            }

            Edge* operator->() const
            {
                return edge;
            }

            //! Checks if an edge iterator is the "dominant" part of a pair of half edges
            bool isUpperHalf() const
            {
                return reinterpret_cast<size_t>(edge)>reinterpret_cast<size_t>(edge->getOpposite());
            }

            VertexPair getVertexPair() const
            {
                return VertexPair(*edge);
            }

            EdgeIterator& advanceFace()
            {
                if((edge=edge->getFaceSucc())==loopStart) // Did we walk around a face once?
                    edge=0; // Mark iterator as invalid
                return *this;
            }

            EdgeIterator& advanceVertex()
            {
                if((edge=edge->getVertexSucc())==loopStart) // Did we walk around a vertex once?
                    edge=0; // Mark iterator as invalid
                return *this;
            }
        };

        //! Iterator for edges
        class ConstEdgeIterator
        {
            friend class PolygonMesh;

            /* Elements: */
          private:
            const Edge* loopStart; // First edge in loop around vertex/face
            const Edge* edge; // Edge pointed to

            /* Constructors and destructors: */
            public:
            ConstEdgeIterator() // Constructs invalid operator
            :loopStart(0),edge(0)
            {
            }

            ConstEdgeIterator(const Edge* sEdge)
            :loopStart(sEdge),edge(sEdge)
            {
            }

            ConstEdgeIterator(const EdgeIterator& eIt)
            :loopStart(eIt.loopStart),edge(eIt.edge)
            {
            }

            /* Methods: */
            friend bool operator==(const ConstEdgeIterator& it1,const ConstEdgeIterator& it2)
            {
                return it1.edge==it2.edge;
            }

            friend bool operator!=(const ConstEdgeIterator& it1,const ConstEdgeIterator& it2)
            {
                return it1.edge!=it2.edge;
            }

            const Edge& operator*() const
            {
                return *edge;
            }

            const Edge* operator->() const
            {
                return edge;
            }

            //1 Checks if an edge iterator is the "dominant" part of a pair of half edges
            bool isUpperHalf() const
            {
                return reinterpret_cast<size_t>(edge)>reinterpret_cast<size_t>(edge->getOpposite());
            }

            VertexPair getVertexPair() const
            {
                return VertexPair(*edge);
            }

            ConstEdgeIterator& advanceFace()
            {
                if((edge=edge->getFaceSucc())==loopStart) // Did we walk around a face once?
                edge=0; // Mark iterator as invalid
                return *this;
            }

            ConstEdgeIterator& advanceVertex()
            {
                if((edge=edge->getVertexSucc())==loopStart) // Did we walk around a vertex once?
                edge=0; // Mark iterator as invalid
                return *this;
            }
        };

        //! Iterator for edges around a vertex
        class VertexEdgeIterator:public EdgeIterator
        {
            friend class PolygonMesh;
            friend class ConstVertexEdgeIterator;

            /* Constructors and destructors: */
          public:
            VertexEdgeIterator() // Constructs invalid iterator
            {
            }

            VertexEdgeIterator(Vertex* vertex) // Constructs an iterator for a vertex
            :EdgeIterator(vertex->getEdge())
            {
            }

            /* Methods: */
            VertexEdgeIterator& operator++()
            {
                EdgeIterator::advanceVertex();
                return *this;
            }

            VertexEdgeIterator operator++(int)
            {
                VertexEdgeIterator result(*this);
                EdgeIterator::advanceVertex();
                return result;
            }
        };

        //! Iterator for edges around a vertex
        class ConstVertexEdgeIterator:public ConstEdgeIterator
        {
            friend class PolygonMesh;

            /* Constructors and destructors: */
            public:
            ConstVertexEdgeIterator() // Constructs invalid iterator
            {
            }

            ConstVertexEdgeIterator(const Vertex* vertex) // Constructs an iterator for a vertex
            :ConstEdgeIterator(vertex->getEdge())
            {
            }

            ConstVertexEdgeIterator(const VertexEdgeIterator& it)
            :ConstEdgeIterator(it)
            {
            }

            /* Methods: */
            ConstVertexEdgeIterator& operator++()
            {
                ConstEdgeIterator::advanceVertex();
                return *this;
            }

            ConstVertexEdgeIterator operator++(int)
            {
                ConstVertexEdgeIterator result(*this);
                ConstEdgeIterator::advanceVertex();
                return result;
            }
        };

        //! Iterator for vertices
        class VertexIterator
        {
            friend class PolygonMesh;
            friend class ConstVertexIterator;

            /* Elements: */
           private:
            Vertex* vertex; // Vertex pointed to

            /* Constructors and destructors: */
            public:
            VertexIterator(void) // Constructs invalid iterator
            :vertex(0)
            {
            }

            VertexIterator(Vertex* sVertex)
            :vertex(sVertex)
            {
            }

            /* Methods: */
            friend bool operator==(const VertexIterator& it1,const VertexIterator& it2)
            {
                return it1.vertex==it2.vertex;
            }

            friend bool operator!=(const VertexIterator& it1,const VertexIterator& it2)
            {
                return it1.vertex!=it2.vertex;
            }

            Vertex& operator*() const
            {
                return *vertex;
            }

            Vertex* operator->() const
            {
                return vertex;
            }

            static size_t hash(const VertexIterator& vertexIt,size_t tableSize)
            {
                return reinterpret_cast<size_t>(vertexIt.vertex)%tableSize;
            }

            VertexEdgeIterator beginEdges() const
            {
                return VertexEdgeIterator(vertex);
            }

            VertexEdgeIterator endEdges() const
            {
                return VertexEdgeIterator();
            }

            Vec3D& getPoint() const
            {
                return *vertex;
            }

            VertexIterator& operator++()
            {
                vertex=vertex->succ;
                return *this;
            }

            VertexIterator operator++(int)
            {
                VertexIterator result(*this);
                vertex=vertex->succ;
                return result;
            }
        };

        //! Iterator for constant vertices
        class ConstVertexIterator
        {
            friend class PolygonMesh;

            /* Elements: */
          private:
            const Vertex* vertex; // Vertex pointed to

            /* Constructors and destructors: */
            public:
            ConstVertexIterator(void) // Constructs invalid iterator
            :vertex(0)
            {
            }

            ConstVertexIterator(const Vertex* sVertex)
            :vertex(sVertex)
            {
            }

            ConstVertexIterator(const VertexIterator& it)
            :vertex(it.vertex)
            {
            }

            /* Methods: */
            friend bool operator==(const ConstVertexIterator& it1,const ConstVertexIterator& it2)
            {
                return it1.vertex==it2.vertex;
            }

            friend bool operator!=(const ConstVertexIterator& it1,const ConstVertexIterator& it2)
            {
                return it1.vertex!=it2.vertex;
            }

            const Vertex& operator*() const
            {
            return *vertex;
            }

            const Vertex* operator->() const
            {
            return vertex;
            }

            static size_t hash(const ConstVertexIterator& vertexIt,size_t tableSize)
            {
                return reinterpret_cast<size_t>(vertexIt.vertex)%tableSize;
            }

            ConstVertexEdgeIterator beginEdges() const
            {
                return ConstVertexEdgeIterator(vertex);
            }

            ConstVertexEdgeIterator endEdges() const
            {
                return ConstVertexEdgeIterator();
            }

            const Vec3D& getPoint() const
            {
                return *vertex;
            }

            ConstVertexIterator& operator++()
            {
                vertex=vertex->succ;
                return *this;
            }

            ConstVertexIterator operator++(int)
            {
                ConstVertexIterator result(*this);
                vertex=vertex->succ;
                return result;
            }
        };

        //! Class to enumerate the edges around a face
        class FaceEdgeIterator:public EdgeIterator
        {
            friend class PolygonMesh;
            friend class ConstFaceEdgeIterator;

            /* Constructors and destructors: */
          public:
            FaceEdgeIterator() // Constructs invalid iterator
            {
            }

            FaceEdgeIterator(Face* face) // Constructs an iterator for a face
            :EdgeIterator(face->getEdge())
            {
            }

            /* Methods: */
            FaceEdgeIterator& operator++()
            {
                EdgeIterator::advanceFace();
                return *this;
            }

            FaceEdgeIterator operator++(int)
            {
                FaceEdgeIterator result(*this);
                EdgeIterator::advanceFace();
                return result;
            }

             /* Methods: */
            friend bool operator==(const FaceEdgeIterator& it1,const EdgeIterator& it2)
            {
            return it1.getCurrent()==it2.getCurrent();
            }

            friend bool operator!=(const FaceEdgeIterator& it1,const EdgeIterator& it2)
            {
            return it1.getCurrent()!=it2.getCurrent();
            }

            FaceEdgeIterator& operator=(const EdgeIterator &other)
            {
                edge = other.getCurrent();
                loopStart = other.getCurrent();
                return *this;
            }
        };

        //!Class to enumerate the edges around a face
        class ConstFaceEdgeIterator:public ConstEdgeIterator
        {
            friend class PolygonMesh;

            /* Constructors and destructors: */
          public:
            ConstFaceEdgeIterator() // Constructs invalid iterator
            {
            }

            ConstFaceEdgeIterator(const Face* face) // Constructs an iterator for a face
            :ConstEdgeIterator(face->getEdge())
            {
            }

            ConstFaceEdgeIterator(const FaceEdgeIterator& it)
            :ConstEdgeIterator(it)
            {
            }

            /* Methods: */
            ConstFaceEdgeIterator& operator++(void)
            {
                ConstEdgeIterator::advanceFace();
                return *this;
            }

            ConstFaceEdgeIterator operator++(int)
            {
                ConstFaceEdgeIterator result(*this);
                ConstEdgeIterator::advanceFace();
                return result;
            }
        };

        //! Iterator for faces
        class FaceIterator
        {
            friend class PolygonMesh;
            friend class ConstFaceIterator;

            /* Elements: */
          private:
            Face* face; // Face pointed to

            /* Constructors and destructors: */
          public:
            FaceIterator() // Constructs invalid iterator
            :face(0)
            {
            }

            FaceIterator(Face* sFace)
            :face(sFace)
            {
            }

            /* Methods: */
            friend bool operator==(const FaceIterator& it1,const FaceIterator& it2)
            {
                return it1.face==it2.face;
            }

            friend bool operator!=(const FaceIterator& it1,const FaceIterator& it2)
            {
                return it1.face!=it2.face;
            }

            FaceIterator& operator =(const FaceIterator& other)
            {
                face = other.getCurrent();
                return *this;
            }

            Face& operator*() const
            {
                return *face;
            }

            Face* operator->() const
            {
             return face;
            }

            Face* getCurrent() const
            {
             return face;
            }

            static size_t hash(const FaceIterator& faceIt,size_t tableSize)
            {
                return reinterpret_cast<size_t>(faceIt.face)%tableSize;
            }

            FaceEdgeIterator beginEdges() const
            {
                return FaceEdgeIterator(face);
            }

            FaceEdgeIterator endEdges() const
            {
                return FaceEdgeIterator();
            }

            FaceIterator& operator++()
            {
                face=face->succ;
                return *this;
            }

            FaceIterator operator++(int)
            {
                FaceIterator result(*this);
                face=face->succ;
                return result;
            }
        };

        //! Iterator for constant faces
        class ConstFaceIterator
        {
            friend class PolygonMesh;

            /* Elements: */
          private:
            const Face* face; // Face pointed to

            /* Constructors and destructors: */
          public:
            ConstFaceIterator() // Constructs invalid iterator
            :face(0)
            {
            }

            ConstFaceIterator(const Face* sFace)
            :face(sFace)
            {
            }

            ConstFaceIterator(const FaceIterator& it)
            :face(it.face)
            {
            }

            /* Methods: */
            friend bool operator==(const ConstFaceIterator& it1,const ConstFaceIterator& it2)
            {
                return it1.face==it2.face;
            }

            friend bool operator!=(const ConstFaceIterator& it1,const ConstFaceIterator& it2)
            {
                return it1.face!=it2.face;
            }

            ConstFaceIterator& operator =(const ConstFaceIterator& other)
            {
                face = other.getCurrent();
                return *this;
            }

            const Face& operator*(void) const
            {
                return *face;
            }

            const Face* operator->(void) const
            {
                return face;
            }

            const Face* getCurrent() const
            {
             return face;
            }

            static size_t hash(const ConstFaceIterator& faceIt,size_t tableSize)
            {
                return reinterpret_cast<size_t>(faceIt.face)%tableSize;
            }

            ConstFaceEdgeIterator beginEdges(void) const
            {
                return ConstFaceEdgeIterator(face);
            }

            ConstFaceEdgeIterator endEdges(void) const
            {
                return ConstFaceEdgeIterator();
            }

            ConstFaceIterator& operator++(void)
            {
                face=face->succ;
                return *this;
            }

            ConstFaceIterator operator++(int)
            {
                ConstFaceIterator result(*this);
                face=face->succ;
                return result;
            }
        };

        /*embedded classes end*/

        typedef HashTable<VertexPair,Edge*,VertexPair> EdgeHasher;
        /* Elements: */
        protected:
        /* Allocators for vertices, edges and faces: */
        PoolAllocator<Vertex> vertexAllocator;
        PoolAllocator<Edge> edgeAllocator;
        PoolAllocator<Face> faceAllocator;

        unsigned int version; // Version number of the entire mesh
        int numVertices; // Number of vertices in the mesh
        Vertex* vertices; // Pointer to head of vertex list
        Vertex* lastVertex; // Pointer to tail of vertex list
        int numEdges; // Number of edges in the mesh
        int numFaces; // Number of faces in the mesh
        Face* faces; // Pointer to head of face list
        Face* lastFace; // Pointer to tail of face list

        /* Protected methods: */
        Vertex* newVertex(const Vec3D& p); // Creates a new vertex
        Vertex* newVertex(const Vec3D& p,const RPColor& c); // Creates a new vertex
        Vertex* newVertex(const Vec3D& p,const RPColor& c,const RPUV &uv); // Creates a new vertex
        void deleteVertex(Vertex* vertex); // Deletes a vertex
        Edge* newEdge(void); // Creates a new edge
        void deleteEdge(Edge* edge); // Deletes an edge
        Face* newFace(void); // Creates a new face
        void deleteFace(Face* face); // Deletes a face

        /* Constructors and destructors: */
        public:
        //! Creates empty mesh
        PolygonMesh(void)
            :version(0),
             numVertices(0),vertices(0),lastVertex(0),
             numEdges(0),
             numFaces(0),faces(0),lastFace(0)
        {
        }

        //! Copies a polygon mesh
        PolygonMesh(const PolygonMesh& source);
        virtual ~PolygonMesh(void);

        /* Methods: */
        VertexIterator addVertex(const Vec3D& pos)
        {
            return VertexIterator(newVertex(pos));
        }

        VertexIterator addVertex(const Vec3D& pos,const RPColor& color)
        {
            return VertexIterator(newVertex(pos,color));
        }

        VertexIterator addVertex(const Vec3D& pos,const RPColor& color,const RPUV &uv)
        {
            return VertexIterator(newVertex(pos,color,uv));
        }

        void clearMesh();
        EdgeHasher* startAddingFaces(void);
        FaceIterator addFace(int numVertices,const VertexIterator vertices[],EdgeHasher* edgeHasher);
        FaceIterator addFace(const std::vector<VertexIterator>& vertices,EdgeHasher* edgeHasher);
        void setEdgeSharpness(VertexIterator v1,VertexIterator v2,int sharpness,EdgeHasher* edgeHasher);
        void finishAddingFaces(EdgeHasher* edgeHasher);
        void alterVertices(Matrix4X4 mat);
        void calFacesZ();
        void setFacesTraveType(short sv);

        int getNumVertices() const
        {
            return numVertices;
        }

        VertexIterator beginVertices()
        {
            return VertexIterator(vertices);
        }

        ConstVertexIterator beginVertices() const
        {
            return ConstVertexIterator(vertices);
        }

        VertexIterator endVertices()
        {
            return VertexIterator(0);
        }

        ConstVertexIterator endVertices() const
        {
            return ConstVertexIterator(0);
        }

        //! Creates a new vertex without adding it to the main vertex list
        Vertex* createVertex(const Vec3D& p,const RPColor& c,const RPUV &suv=RPUV(0.0,0.0),Vertex* succ =0)
        {
            return new(vertexAllocator.allocate()) Vertex(p,c,suv,succ);
        }

        int getNumEdges() const
        {
            return numEdges;
        }

        int getNumFaces() const
        {
            return numFaces;
        }

        FaceIterator beginFaces()
        {
            return FaceIterator(faces);
        }

        ConstFaceIterator beginFaces() const
        {
            return ConstFaceIterator(faces);
        }

        FaceIterator endFaces()
        {
            return FaceIterator(0);
        }

        ConstFaceIterator endFaces() const
        {
            return ConstFaceIterator(0);
        }

        // Recalculates the normal vectors of all updated vertices
        void updateVertexNormals(void);

        void flipNormals(void);

        //! key is the even elevation of the layer.
        float getKey();

        // Marks all vertices as up-to-date
        void validateVertices(void)
        {
            ++version;
        }

        // Invalidates a vertex
        void invalidateVertex(const VertexIterator& vertexIt)
        {
            vertexIt->version=version;
        }

        // Removes a singular vertex (a vertex without edges)
        void removeSingularVertex(const VertexIterator& vertexIt);
        // Removes a vertex by turning its platelet into a hole
        void removeVertex(const VertexIterator& vertexIt);
        // Converts a vertex into a face by splitting triangles off all adjacent faces
        FaceIterator vertexToFace(const VertexIterator& vertexIt);
        // Splits an existing edge using an existing unlinked vertex
        VertexIterator splitEdge(const EdgeIterator& edgeIt,Vertex* edgePoint);

        //! Splits an existing edge by creating a new vertex
        VertexIterator splitEdge(const EdgeIterator& edgeIt,const Vec3D& p,const RPColor& c)
        {
            return splitEdge(edgeIt,createVertex(p,c));
        }

        // Rotates an edge in counter-clockwise direction
        void rotateEdge(const EdgeIterator& edgeIt);
        // Joins two faces by removing an edge
        FaceIterator removeEdge(const EdgeIterator& edgeIt);

        // Returns the sharpness of an edge
        int getEdgeSharpness(const EdgeIterator& edgeIt)
        {
            return edgeIt->sharpness;
        }

        // Sets the sharpness of an edge
        void setEdgeSharpness(const EdgeIterator& edgeIt,int newSharpness)
        {
            edgeIt->sharpness=newSharpness;
            if(edgeIt->getOpposite()!=0)
                edgeIt->getOpposite()->sharpness=newSharpness;
        }

        // Removes a face by replacing it with a hole
        void removeFace(const FaceIterator& faceIt);
        // Splits a face into triangles by inserting new edges
        void triangulateFace(const FaceIterator& faceIt);
        // Splits a face by connecting two points by a new edge
        EdgeIterator splitFace(const VertexIterator& vIt1,const VertexIterator& vIt2);
        // Splits an existing face into a triangle fan around the existing unlinked vertex
        VertexIterator splitFace(const FaceIterator& faceIt,Vertex* facePoint);

        // Splits an existing face into a triangle fan around the new vertex
        template <class InputPointType>
        VertexIterator splitFace(const FaceIterator& faceIt,const InputPointType& p)
        {
            return splitFace(faceIt,createVertex(p));
        }

        // Ditto, uses existing unlinked vertex
        VertexIterator splitFaceCatmullClark(const FaceIterator& faceIt,Vertex* v);

        // Splits an existing face into a quad fan around the new vertex
        template <class InputPointType>
        VertexIterator splitFaceCatmullClark(const FaceIterator& faceIt,const InputPointType& p)
        {
            return splitFaceCatmullClark(faceIt,createVertex(p));
         }

        // Insets a face as in Doo-Sabin subdivision
        FaceIterator splitFaceDooSabin(const FaceIterator& faceIt);

        };


}
#endif // POLYGON_MESH_H
