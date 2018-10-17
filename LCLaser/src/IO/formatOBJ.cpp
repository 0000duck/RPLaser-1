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
#include "formatOBJ.h"

using namespace std;
using namespace IORP;

typedef vector<DataRP::PolygonMesh::VertexIterator> FaceInterVertices;

bool OBJData::parseOBJ(QString m_fileName)
{
    struct VertexInfo
    {
        float v[3];
        VertexInfo()
        {
            memset(v,0,sizeof(float)*3);
        }
    };
    struct VertexDefine
    {
        int positionIndex;
        int texcoordIndex;
        int normalIndex;
    };
    std::vector<VertexInfo> positions;//顶点坐标
    std::vector<VertexInfo> texcoords;//纹理坐标
    std::vector<VertexInfo> normals;//法线
    std::vector<VertexInfo> faces;//法线
    std::vector<VertexDefine> vertices;


    QByteArray filename = m_fileName.toLocal8Bit();
    ifstream objHeadStream(filename.data(),ios::in);
    if(!objHeadStream.is_open())
    {
        return false;
    }
    //解析模型文件
    //stringstream是字符串流 ,它将流与存储在内存中的string对象绑定起来
    std::stringstream ssFileContent;
    char szOneLine[256];
    std::string temp;
    DATAOBJ objdata;

    while (!objHeadStream.eof())
    {
        memset(szOneLine,0,256);
        objHeadStream.getline(szOneLine,256);
        ssFileContent.str(szOneLine);
        if (strlen(szOneLine)>0)
        {
            std::stringstream ssOneLine(szOneLine);
            if (szOneLine[0]=='v')//解析顶点信息
            {
                  if (szOneLine[1] == 't')//解析纹理坐标
                    {
                      VertexInfo vi;
                      ssOneLine >> temp;//vt
                      ssOneLine >> vi.v[0];
                      ssOneLine >> vi.v[1];
                      texcoords.push_back(vi);
                    }
                    else if (szOneLine[1] == 'n')//解析法线坐标
                    {
                      VertexInfo vi;
                      ssOneLine >> temp;//vn
                      ssOneLine >> vi.v[0];
                      ssOneLine >> vi.v[1];
                      ssOneLine >> vi.v[2];
                      normals.push_back(vi);
                    }
                    else //解析顶点信息
                    {
                      VertexInfo vi;
                      ssOneLine >> temp;//v
                      ssOneLine >> vi.v[0];
                      ssOneLine >> vi.v[1];
                      ssOneLine >> vi.v[2];
                      positions.push_back(vi);
                      //cout<<objdata.position[0]<<endl;
                    }
            }
            else if (szOneLine[0]=='f') //解析面信息
            {
                ssOneLine >> temp;//f
                std::string vertexStr;
                VertexInfo vi;
                for (int i = 0; i < 3; ++i)
                {
                    ssOneLine >> vertexStr;
                    size_t pos = vertexStr.find_first_of('/');
                    std::string positionIndexStr = vertexStr.substr(0, pos);
                    size_t pos2 = vertexStr.find_first_of('/', pos + 1);
                    std::string texcoordIndexStr = vertexStr.substr(pos + 1, pos2 - pos - 1);
                    std::string normalIndexStr = vertexStr.substr(pos2 + 1, vertexStr.length() - pos2 - 1);
                    VertexDefine vd;
                    vd.positionIndex = atoi(positionIndexStr.c_str()) - 1;
                    vd.texcoordIndex = atoi(texcoordIndexStr.c_str()) - 1;
                    vd.normalIndex = atoi(normalIndexStr.c_str()) - 1;
                    //trim the same vertice
                    int nCurrentVertexIndex = -1;
                    size_t nCurrentVerticeCount = vertices.size();
                    for (int j = 0; j<nCurrentVerticeCount; ++j)
                    {
                        if (vertices[j].positionIndex == vd.positionIndex&&
                            vertices[j].texcoordIndex == vd.texcoordIndex&&
                            vertices[j].normalIndex == vd.normalIndex)
                        {
                            nCurrentVertexIndex = j;
                            break;
                        }
                    }
                    if (nCurrentVertexIndex == -1)
                    {
                        nCurrentVertexIndex = (int)vertices.size();
                        vertices.push_back(vd);
                    }
                    vi.v[i]=nCurrentVertexIndex;
                }
                faces.push_back(vi);
            }
        }
    }
    m_faceNumb=faces.size();
//    qDebug()<<"The number of face is "<<faces.size()<<endl;
//    qDebug()<<"The position is "<<faces[0].v[0]<<endl;
//    qDebug()<<"The position is "<<faces[0].v[1]<<endl;
//    qDebug()<<"The position is "<<faces[0].v[2]<<endl;
    for(int i = 0;i<faces.size();i++)
    {
        float vetex[3]={0,0,0};
        //float normal[3]={0,0,0};
        //float texcoord[3]={0,0,0};
        for(int j = 0; j < 3;j++)
        {
            for(int k = 0; k < 3;k++)
            {
                vetex[k]=positions[faces[i].v[j]].v[k];
            }
            objdata.setComponent(vetex,j);
            //normal[j]=normals[i].v[j];
            //texcoord[j]=texcoords[i].v[j];
        }
        m_objdata.push_back(objdata);
    }
    //m_objdata[0].printMe();
    return true;
}
void OBJData::finishAddTriangles()
{
    mesh->finishAddingVertices(vertexHasher);
    mesh->finishAddingFaces(edgeHasher);
    //mesh->updateVertexNormals();
    mesh->calFacesZ();
}

void OBJData::obj2Mesh(DataRP::TriangleMesh *triMesh)
{
    mesh = triMesh;
    Vec3D vec[3];
    OBJDATA::iterator faceIt;
    DataRP::PolygonMesh::VertexIterator vertexIts[3];
    DataRP::TriangleMesh::VertexHasher::Iterator verHashIt;
    vertexHasher = mesh->startAddingVertex();
    edgeHasher = mesh->startAddingFaces();

    double minmax[3][2]; //minX,maxX  minY,maxY, minZ,maxZ
    m_objdata[0].getVertices(vec);

    for(int i = 0; i < 2; i++)
    {
        minmax[0][i] = vec[i].getX();
        minmax[1][i] = vec[i].getY();
        minmax[2][i] = vec[i].getZ();
    }


    for(faceIt = m_objdata.begin(); faceIt != m_objdata.end(); faceIt++)
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

RPIOobj::RPIOobj(DataRP::RPData *pdata)
{
    rp_data = pdata;
}

bool RPIOobj::readOBJfile(QString objfileName)
{
    DataRP::TriangleMesh *objmesh= rp_data->createNewMesh();
    OBJData objdata(objfileName);
    if(!objdata.parseOBJ(objfileName))
    {
       cout<<"cannot read obj data"<<endl;
       return false;
    }
    float facenumb = objdata.getFaceNumb();
    cout<<"face number is  "<<facenumb<<endl;
    objdata.obj2Mesh(objmesh);
    rp_data->refresh();
    return true;
}



