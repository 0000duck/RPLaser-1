/***************************************************************************
                            formatSTL.cpp  -  description
                             -------------------
    begin                : Oct. 28 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
    descrition           : providing base polygon elements
 ***************************************************************************/
#include <string.h>
#include <QDebug>
#include "formatSTL.h"

using namespace std;
using namespace IORP;

string STLFormat::s_asciiMark = "solid ";
int STLFormat::s_facenumbpos = 80;
int STLFormat::s_datapos = 84;
int STLFormat::s_binaryByte = 50;

typedef vector<DataRP::PolygonMesh::VertexIterator> FaceInterVertices;

bool STLhead::parseSTL()
{
    int bufflen = 256;
    char buff[256];
    memset(buff,0,bufflen);
    QByteArray filename = m_fileName.toLocal8Bit();
    ifstream stlHeadStream(filename.data(),ios::in);
    stringstream strStream;
    string fistLine;
    string featureStr;

    if(!stlHeadStream.is_open())
    {
        return false;
    }

    stlHeadStream.getline(buff,bufflen);
    strStream.clear();
    strStream.str(buff);
    stlHeadStream.close();

    strStream>>fistLine;

    featureStr = fistLine.substr(0,5);
    if(featureStr == STLhead::s_asciiMark)
    {
        m_stlType = STL_ASCII;
    }
    else
    {
        m_stlType = STL_BINARY;
    }

    //check STL type.
    if(m_stlType == STL_BINARY)
    {
        stlHeadStream.clear();
        QByteArray filename = m_fileName.toLocal8Bit();
        stlHeadStream.open(filename.data(),ios::in|ios::binary);

        if(!stlHeadStream.is_open())
        {
            return false;
        }

        stlHeadStream.seekg(s_facenumbpos,ios::beg);
        stlHeadStream.read(reinterpret_cast<char*>(&m_faceNumb),4);
        stlHeadStream.close();
    }

    return true;
}

bool STLData::parseSTL()
{
    if(m_stlType == STL_BINARY)
    {
        return parseBinary();
    }
    else if(m_stlType == STL_ASCII)
    {
        return parseASCII();
    }
    qDebug()<<"cannot recognize format"<<endl;
    return false;
}

bool STLData::parseASCII()
{
    QByteArray filename = m_fileName.toLocal8Bit();
    ifstream stlASCIIStream(filename.data(),ios::in);
    if(!stlASCIIStream.is_open())
    {
        return false;
    }
    int MaxLen = 1024;
    char buff[1024];
    string mark;
    float value[3];
    BinarySTL dataItem;
    unsigned int cnt = 0;

    while(!stlASCIIStream.eof())
    {
        stlASCIIStream.getline(buff,MaxLen);
        stringstream strStream(buff);
        qDebug()<<"line :"<<buff<<endl;
        strStream>>mark>>value[0]>>value[1]>>value[2];
        qDebug()<<QString::fromStdString(mark)<<endl;

        if(mark == "vertex")
        {
            dataItem.setComponent(value,cnt);
            cnt++;
            if(cnt == 3)
            {
                m_stlbinary.push_back(dataItem);
                cnt = 0;
            }
        }
        else
        {
            if(cnt != 0)
            {
                qDebug()<<"ascii except"<<endl;
            }
            cnt = 0;
        }
    }
    qDebug()<<"number of ASCII faces"<<m_stlbinary.size()<<endl;
    return true;
}

bool STLData::parseBinary()
{
    QByteArray filename = m_fileName.toLocal8Bit();
    ifstream stlDataStream(filename.data(),ios::in|ios::binary);

    if(!stlDataStream.is_open())
    {
        return false;
    }
    stlDataStream.seekg(0,ios::end);// 定位到文件结束处

    double length =  stlDataStream.tellg();//文件大小
    //qDebug()<<"size of fill is  "<<length<<endl;
    length -= s_datapos;

    stlDataStream.clear();
    stlDataStream.seekg(s_datapos,ios::beg);//输入流的开始

    int dataSize = (length)/s_binaryByte;//三角形面片数
    m_stlbinary.resize(dataSize);
    stlDataStream.read(reinterpret_cast<char*>(&m_stlbinary[0]),length);
    stlDataStream.close();
    m_stlbinary[0].printMe();
    qDebug()<<"number of binary faces"<<m_stlbinary.size()<<endl;
    return true;
}

void STLData::finishAddTriangles()
{
    mesh->finishAddingVertices(vertexHasher);
    mesh->finishAddingFaces(edgeHasher);
    //mesh->updateVertexNormals();
    mesh->calFacesZ();
}

void STLData::stl2Mesh(DataRP::TriangleMesh *triMesh)
{
    mesh = triMesh;
    Vec3D vec[3];
    STLBinary::iterator faceIt;
    DataRP::PolygonMesh::VertexIterator vertexIts[3];

    DataRP::TriangleMesh::VertexHasher::Iterator verHashIt;
    vertexHasher = mesh->startAddingVertex();
    edgeHasher = mesh->startAddingFaces();

    double minmax[3][2]; //minX,maxX  minY,maxY, minZ,maxZ
    m_stlbinary[0].getVertices(vec);
    for(int i = 0; i < 2; i++)
    {
        minmax[0][i] = vec[i].getX();
        minmax[1][i] = vec[i].getY();
        minmax[2][i] = vec[i].getZ();
    }


    for(faceIt = m_stlbinary.begin(); faceIt != m_stlbinary.end(); faceIt++)
    {
        faceIt->getVertices(vec);
        for(unsigned int i = 0; i < 3; ++i) //build vertices
        {
           verHashIt = vertexHasher->findEntry(vec[i]);
           if(!verHashIt.isFinished())//vertex already exist.
           {
               vertexIts[i] = verHashIt->getDest();
           }
           else                       //new vertex
           {
               vertexIts[i] = triMesh->addVertex(vec[i]);
               vertexHasher->setEntry(DataRP::TriangleMesh::VertexHasher::Entry(vec[i],vertexIts[i]));

               if(vec[i].getX() < minmax[0][0])
                   minmax[0][0] = vec[i].getX();

               if(vec[i].getX() > minmax[0][1])
                   minmax[0][1] = vec[i].getX();

               if(vec[i].getY() < minmax[1][0])
                   minmax[1][0] = vec[i].getY();

               if(vec[i].getY() > minmax[1][1])
                   minmax[1][1] = vec[i].getY();

               if(vec[i].getZ() < minmax[2][0])
                   minmax[2][0] = vec[i].getZ();

               if(vec[i].getZ() > minmax[2][1])
                   minmax[2][1] = vec[i].getZ();
           }
        }

        mesh->addFace(3,vertexIts,edgeHasher);
    }
    finishAddTriangles();
    mesh->m_bbox.setBounding(minmax[0][0],minmax[0][1],minmax[1][0],
                             minmax[1][1],minmax[2][0],minmax[2][1]);

}

void STLData::Mesh2stl(DataRP::TriangleMesh *triMesh,STLBinary &stlbinary)
{
    Vec3D point;
    stlbinary.clear();
    BinarySTL item;
    int vertexIndex = 0;
    for(DataRP::TriangleMesh::ConstFaceIterator fIt = triMesh->beginFaces(); fIt != triMesh->endFaces(); ++fIt)
    {
        vertexIndex = 0;
        point = fIt->getNormal();
        item.setNormal(point);
        for(DataRP::TriangleMesh::ConstFaceEdgeIterator feIt = fIt.beginEdges(); feIt != fIt.endEdges(); ++feIt)
        {

           point = (*feIt->getStart());
           item.setComponent(point,vertexIndex);

           if(vertexIndex > 3)
               continue;
        }
        stlbinary.push_back(item);
    }
}

RPIOstl::RPIOstl(DataRP::RPData *pdata)
{
    rp_data = pdata;
}

bool RPIOstl::readSTLfile(QString stlfileName)
{
    STLhead stlheader(stlfileName);
    DataRP::TriangleMesh *stlmesh= rp_data->createNewMesh();

    if(!stlheader.parseSTL())
    {
       cout<<"cannot read stl head"<<endl;
       return false;
    }

    float facenumb = stlheader.getFaceNumb();
    cout<<"face number is  "<<facenumb<<endl;
    STLData stldata(stlheader);
    if(!stldata.parseSTL())
    {
       cout<<"cannot read stl data"<<endl;
       return false;
    }    
    stldata.stl2Mesh(stlmesh);
    rp_data->refresh();
    return true;
}

bool RPIOstl::saveSTLfile(QString stlfileName,int stlType)
{
    if(stlType == STL_ASCII)
    {
        return saveSTLASCII(stlfileName);
    }
    else  if(stlType == STL_BINARY)
    {
        return saveSTLBinary(stlfileName);
    }
        return false;
}

bool RPIOstl::saveSTLASCII(QString stlfileName)
{
    Vec3D point;
    QByteArray filename = stlfileName.toLocal8Bit();
    ofstream out(filename.data());

    if (!out.is_open())
        return false;

    DataRP::TriangleMesh *mesh = rp_data->getMesh();

    if(mesh == NULL)
        return false;

    out<<"solid RPLaser STL"<<endl;
    for(DataRP::TriangleMesh::ConstFaceIterator fIt = mesh->beginFaces(); fIt != mesh->endFaces(); ++fIt)
    {
        out<<"  facet normal "<<"0 0 0"<<endl;
        out<<"    outer loop"<<endl;
        for(DataRP::TriangleMesh::ConstFaceEdgeIterator feIt = fIt.beginEdges(); feIt != fIt.endEdges(); ++feIt)
        {
            point = (*feIt->getStart());
            out<<"        vertex "<<point.getX()<<" "<<point.getY()<<" "<<point.getZ()<<endl;
        }
        out<<"    endloop"<<endl;
        out<<"  endfacet"<<endl;
    }
    out<<"endsolid RPLaser STL"<<endl;
    out.close();
    return true;
}

bool RPIOstl::saveSTLBinary(QString stlfileName)
{
    QByteArray filename = stlfileName.toLocal8Bit();
    ofstream out(filename.data(),ios::out|ios::binary);

    if(!out.is_open())
    {
        return false;
    }

    unsigned short facenumb = 0, verticesLen = 0;
    int facenumbSize = STLFormat::s_datapos - STLFormat::s_facenumbpos;
    char buf[256];
    STLData stlData;
    STLBinary stlbinary;
    DataRP::TriangleMesh *mesh = rp_data->getMesh();

    memset(buf,0,256);
    sprintf(buf,"%s","RPLaser STL");
    out.write(buf,STLFormat::s_facenumbpos);

    facenumb = mesh->getNumVertices();
    out.write(reinterpret_cast<char*>(&facenumb),facenumbSize);

   stlData.Mesh2stl(mesh,stlbinary);
   verticesLen = stlbinary.size()*50;
   out.write(reinterpret_cast<char*>(&stlbinary[0]),verticesLen);
   out.close();

   return true;
}
