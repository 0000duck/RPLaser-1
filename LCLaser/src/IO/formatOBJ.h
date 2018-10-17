/***************************************************************************
                            formatSTL.h  -  description
                             -------------------
    begin                : Oct. 28 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
    descrition           : providing base polygon elements
 ***************************************************************************/

#ifndef FORMAT_OBJ_H
#define FORMAT_OBJ_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "data.h"

namespace IORP
{

class OBJFormat
{
    public:
    OBJFormat():m_faceNumb(0)
    {
    }

    OBJFormat(QString filename):m_fileName(filename),m_faceNumb(0)
    {
    }

    virtual ~OBJFormat()
    {
    }

    void getParam(const OBJFormat &other)
    {
        m_fileName = other.getFileName();
        m_faceNumb = other.getFaceNumb();
    }

    int getFaceNumb() const
    {
        return m_faceNumb;
    }

    QString getFileName()const
    {
        return m_fileName;
    }

    virtual bool parseOBJ(QString m_fileName) = 0;

    protected:
        QString m_fileName;
        int m_faceNumb;

};

#pragma pack(push)
#pragma pack(2)
struct DATAOBJ
{
    float normal[3];
    float component[3][3];

    DATAOBJ()
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
        qDebug()<<"triangle: ";

        qDebug()<<"normal: ";
        for(i = 0; i < 3;i++)
        {
            qDebug()<<normal[i]<<", ";
        }
        qDebug()<<" ";

        qDebug()<<"points: ";
        for(i = 0; i < 3;i++)
        {
            for(j = 0; j < 3; j++)
            {
                qDebug()<<component[i][j]<<", ";
            }
            qDebug()<<" "<<endl;
        }
    }
};
#pragma pack(pop)

typedef vector<DATAOBJ> OBJDATA;

class OBJData: public OBJFormat
{

    public:
        OBJData(){}

        OBJData(QString sfilename):OBJFormat(sfilename)
        {
        }

        ~OBJData()
        {
        }

        virtual bool parseOBJ(QString sfileName);
        void obj2Mesh(DataRP::TriangleMesh *triMesh);
        //void Mesh2obj(DataRP::TriangleMesh *triMesh,OBJDATA &objdata);

    protected:
        void finishAddTriangles();

    private:
        OBJDATA m_objdata;
        DataRP::TriangleMesh::EdgeHasher * edgeHasher;
        DataRP::TriangleMesh::VertexHasher *vertexHasher;
        DataRP::TriangleMesh* mesh;

};

class RPIOobj
{
    public:
        RPIOobj(){}

        RPIOobj(DataRP::RPData *pdata);

        ~RPIOobj(){}

        bool readOBJfile(QString objfileName);
        //bool saveOBJfile(QString objfileName);

    protected:
        //bool saveOBJDATA(QString objfileName);

    private:
        DataRP::RPData *rp_data;
    };
}

#endif //FORMAT_OBJ_H

