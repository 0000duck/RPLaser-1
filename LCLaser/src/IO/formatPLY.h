#ifndef FORMATPLY_H
#define FORMATPLY_H


#include <iostream>
#include <sstream>
#include <fstream>
#include "data.h"

namespace IORP
{
    enum PLYFILETYPE
    {
        PLY_INDET = 0,
        PLY_ASCII,
        PLY_BINARY,
    };

    class PLYFormat
    {
    public:
        PLYFormat():m_plyType(0),m_faceNumb(0),m_vertexNumb(0),m_propertyNumb(0)
        {
        }

        PLYFormat(QString filename):m_fileName(filename),m_plyType(0),m_faceNumb(0),m_vertexNumb(0),m_propertyNumb(0)
        {
        }

        virtual ~PLYFormat()
        {
        }

        void getParam(const PLYFormat &other)
        {
            m_fileName = other.getFileName();
            m_plyType = other.getPLYtype();
            m_faceNumb = other.getFaceNumb();
            m_vertexNumb = other.getVertexNumb();
            m_propertyNumb = other.getPropertyNumb();
        }

         int getPLYtype() const
        {
            return m_plyType;
        }

        int getFaceNumb() const
        {
            return m_faceNumb;
        }
        int getVertexNumb() const
        {
            return m_vertexNumb;
        }
        int getPropertyNumb() const
        {
            return m_propertyNumb;
        }

        QString getFileName()const
        {
            return m_fileName;
        }

        virtual bool parsePLY() = 0;

    public:
        static string s_asciiMark;

    protected:
        QString m_fileName;
        int m_plyType;
        int m_faceNumb;
        int m_vertexNumb;
        int m_propertyNumb;
    };

    class PLYhead: public PLYFormat
    {
    public:
        PLYhead(QString sfilename):PLYFormat(sfilename)
        {
        }

        ~PLYhead()
        {
        }

        virtual bool parsePLY();
    };

    #pragma pack(push)
    #pragma pack(2)
    struct BinaryPLY
    {
        float normal[3];
        float component[3][3];

        BinaryPLY()
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
        }
    };
    #pragma pack(pop)

    typedef vector<BinaryPLY> PLYBinary;

    class PLYData: public PLYFormat
    {

    public:
        PLYData(){}

        PLYData(QString sfilename):PLYFormat(sfilename)
        {
        }

        PLYData(const PLYhead &head)
        {
            getParam(head);
        }

        ~PLYData()
        {
        }

        virtual bool parsePLY();
        void ply2Mesh(DataRP::TriangleMesh *triMesh);

    protected:
        bool parseASCII();
        bool parseBinary();
        void finishAddTriangles();

    private:
        PLYBinary m_plybinary;
        DataRP::TriangleMesh::EdgeHasher * edgeHasher;
        DataRP::TriangleMesh::VertexHasher *vertexHasher;
        DataRP::TriangleMesh* mesh;

    };

    class RPIOply
    {
    public:
        RPIOply(){}

        RPIOply(DataRP::RPData *pdata);

        ~RPIOply(){}

        bool readPLYfile(QString plyfileName);

    private:
        DataRP::RPData *rp_data;
    };
}

#endif //FORMAT_PLY_H
