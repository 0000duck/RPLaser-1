/***************************************************************************
                            formatSTL.h  -  description
                             -------------------
    begin                : Oct. 28 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
    descrition           : providing base polygon elements
 ***************************************************************************/
#ifndef FORMAT_STL_H
#define FORMAT_STL_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "data.h"

namespace IORP
{
    enum STLFILETYPE
    {
        STL_INDET = 0,
        STL_ASCII,
        STL_BINARY,
    };

    class STLFormat
    {
    public:
        STLFormat():m_stlType(0),m_faceNumb(0)
        {
        }

        STLFormat(QString filename):m_fileName(filename),m_stlType(0),m_faceNumb(0)
        {
        }

        virtual ~STLFormat()
        {
        }

        void getParam(const STLFormat &other)
        {
            m_fileName = other.getFileName();
            m_stlType = other.getSTLtype();
            m_faceNumb = other.getFaceNumb();
        }

         int getSTLtype() const
        {
            return m_stlType;
        }

        int getFaceNumb() const
        {
            return m_faceNumb;
        }

        QString getFileName()const
        {
            return m_fileName;
        }

        virtual bool parseSTL() = 0;

    public:
        static string s_asciiMark;
        static int s_facenumbpos;
        static int s_datapos;
        static int s_binaryByte;

    protected:
        QString m_fileName;
        int m_stlType;
        int m_faceNumb;
    };

    class STLhead: public STLFormat
    {
    public:
        STLhead(QString sfilename):STLFormat(sfilename)
        {
        }

        ~STLhead()
        {
        }

        virtual bool parseSTL();
    };

    #pragma pack(push)
    #pragma pack(2)
    struct BinarySTL
    {
        float normal[3];
        float component[3][3];
        unsigned short property;

        BinarySTL()
        {
            int i,j;
            for(i = 0; i < 3;i++)
            {
                normal[i] = 0;
            }

            for(i = 0; i < 3;i++)
            {
                 for(j = 0; j < 3; j++)
                {
                    component[i][j] = 0;
                }
            }

           property = 0;
        }

        void setNormal(const float (&snormal)[3])
        {
            for(int i = 0; i < 3;i++)
            {
                normal[i] = snormal[i];
            }
        }

        void setNormal(Vec3D snormal)
        {
            normal[0] = snormal.getX();normal[1] = snormal.getY();normal[2] = snormal.getZ();
        }

        void setComponent(const float (&vetex)[3],int index)
        {
            for(int i = 0; i < 3;i++)
            {
                component[index][i] = vetex[i];
            }
        }

        void setComponent(Vec3D vertex,int index)
        {
            component[index][0] = vertex.getX();  component[index][1] = vertex.getY();  component[index][2] = vertex.getZ();
        }

        void getVertices(Vec3D (&vec3)[3])
        {
            for(int i = 0; i < 3;i++)
            {
                vec3[i] = Vec3D(component[i][0],component[i][1],component[i][2]);
            }
        }

        void printMe()
        {
            int i,j;
            cout<<"triangle: "<<endl;

            cout<<"normal: ";
            for(i = 0; i < 3;i++)
            {
                cout<<normal[i]<<", ";
            }
            cout<<" "<<endl;

             cout<<"points: ";
            for(i = 0; i < 3;i++)
            {
                 for(j = 0; j < 3; j++)
                {
                    cout<<component[i][j]<<", ";
                }
                 cout<<" "<<endl;
            }

            cout<<"property: "<<property<<endl;
        }
    };
    #pragma pack(pop)

    typedef vector<BinarySTL> STLBinary;

    class STLData: public STLFormat
    {

    public:
        STLData(){}

        STLData(QString sfilename):STLFormat(sfilename)
        {
        }

        STLData(const STLhead &head)
        {
            getParam(head);
        }

        ~STLData()
        {
        }

        virtual bool parseSTL();
        void stl2Mesh(DataRP::TriangleMesh *triMesh);
        void Mesh2stl(DataRP::TriangleMesh *triMesh,STLBinary &stlbinary);

    protected:
        bool parseASCII();
        bool parseBinary();
        void finishAddTriangles();

    private:
        STLBinary m_stlbinary;
        DataRP::TriangleMesh::EdgeHasher * edgeHasher;
        DataRP::TriangleMesh::VertexHasher *vertexHasher;
        DataRP::TriangleMesh* mesh;

    };

    class RPIOstl
    {
    public:
        RPIOstl(){}

        RPIOstl(DataRP::RPData *pdata);

        ~RPIOstl(){}

        bool readSTLfile(QString stlfileName);
        bool saveSTLfile(QString stlfileName,int stlType);

        protected:
        bool saveSTLASCII(QString stlfileName);
        bool saveSTLBinary(QString stlfileName);

    private:
        DataRP::RPData *rp_data;
    };
}

#endif //FORMAT_STL_H
