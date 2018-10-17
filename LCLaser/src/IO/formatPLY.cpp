#include <string.h>
#include <QDebug>
#include <stdlib.h>
#include "formatPLY.h"

using namespace std;
using namespace IORP;

string PLYFormat::s_asciiMark = "ascii";

typedef vector<DataRP::PolygonMesh::VertexIterator> FaceInterVertices;

bool PLYhead::parsePLY()
{
    int bufflen = 256;
    char buff[256];
    QByteArray filename = m_fileName.toLocal8Bit();
    ifstream plyHeadStream(filename.data(),ios::in);
    stringstream strStream;
    string Line;
    string featureStr;

    if(!plyHeadStream.is_open())
    {
        return false;
    }
    while(!plyHeadStream.eof()){
        memset(buff,0,bufflen);
        plyHeadStream.getline(buff,bufflen);
        strStream.str(buff);
        Line=buff;
        //cout<<Line<<endl;
        if(Line.find("format")==0)
        {
            featureStr=Line.substr(7,5);
        }
        if(Line.find("element face")==0)
        {
            stringstream numStream(Line.substr(12,Line.size()-12));
            numStream>>m_faceNumb;
            //cout<<m_faceNumb<<endl;
        }
        if(Line.find("element vertex")==0)
        {
            stringstream numStream(Line.substr(14,Line.size()-14));
            numStream>>m_vertexNumb;
            //cout<<m_vertexNumb<<endl;
        }
        if(Line.find("property")==0)
        {
            m_propertyNumb++;
        }
    }
    strStream.clear();
    plyHeadStream.close();
    if(featureStr == PLYhead::s_asciiMark)
    {
        m_plyType = PLY_ASCII;
    }
    else
    {
        m_plyType = PLY_BINARY;
    }
    return true;
}


bool PLYData::parsePLY()
{
    if(m_plyType == PLY_BINARY)
    {
        return parseBinary();
    }
    else if(m_plyType == PLY_ASCII)
    {
        return parseASCII();

    }
    qDebug()<<"cannot recognize format"<<endl;
    return false;
}


bool PLYData::parseASCII()
{
    struct VertexInfo
    {
        float v[3];
        VertexInfo()
        {
            memset(v,0,sizeof(float)*3);
        }
    };
    std::vector<VertexInfo> positions;//顶点坐标
    std::vector<VertexInfo> faces;//面关系
    BinaryPLY binaryply;
    QByteArray filename = m_fileName.toLocal8Bit();
    ifstream plyASCIIStream(filename.data(),ios::in);
    if(!plyASCIIStream.is_open())
    {
        return false;
    }
    string filein;
    int count=0;
    while(!plyASCIIStream.eof())
    {
        plyASCIIStream>>filein;
        if(filein=="ply"||filein=="PLY")
        {
            //cout<<filein<<endl;//this is the header of .ply file,add TODO here
        }else if(filein=="comment")
        {   //this is the comment,add TODO here
            getline(plyASCIIStream,filein,'\n');
        }else if(filein=="format")
        {   //this is the format information,add TODO here
            getline(plyASCIIStream,filein,'\n');
        }
        else if(filein=="element")
        {   //element information,only accept vertex and face.ignore user-defined element
            plyASCIIStream>>filein;
            if(filein=="vertex")
            {   //read the number of vertex
                plyASCIIStream>>m_vertexNumb;
            }else if(filein=="face")
            {   //read the number of face
                plyASCIIStream>>m_faceNumb;
                //ignore this and the next lines
                getline(plyASCIIStream,filein,'\n');
                getline(plyASCIIStream,filein,'\n');
            }
        }
        else if(filein=="property")
        {   //property,only accept xyz/nxyz/rgb
            plyASCIIStream>>filein;//read in type of element
            count++;
        }
        else if(filein=="end_header")
        {   //header end,add TODO here            
            //read vertex information
            VertexInfo vi;
            for(int i=0;i<m_vertexNumb;i++)//接下来的m_vertexNumb行都是顶点信息
            {
                float tem[20];
                for(int j=0;j<count;j++)
                {
                    plyASCIIStream>>tem[j];
                }
                vi.v[0]=tem[0];
                vi.v[1]=tem[1];
                vi.v[2]=tem[2];
                positions.push_back(vi);
            }
            cout<<"the number of the vertex:"<<positions.size()<<endl;
            for(int i=0;i<positions.size();i++)
            {
                for(int j=0;j<3;j++)
                    cout<<positions[i].v[j]<<",";
                cout<<endl;
            }
            //read face information
            int front=0,i=0;;
            int *indices;/*index of face,indicate the index of vertex that each face contains*/
            if(m_faceNumb)
                indices=new int[m_faceNumb*3];
            plyASCIIStream>>front;
            while(front==3 && i<m_faceNumb)
            {
                plyASCIIStream>>indices[i*3]>>indices[i*3+1]>>indices[i*3+2];
                plyASCIIStream>>front;
                vi.v[0]=indices[i*3];
                vi.v[1]=indices[i*3+1];
                vi.v[2]=indices[i*3+2];
                faces.push_back(vi);
                i++;
            }
            cout<<"the number of the face:"<<faces.size()<<endl;
            for(int i=0;i<faces.size();i++)
            {
                for(int j=0;j<3;j++)
                    cout<<faces[i].v[j]<<",";
                cout<<endl;
            }
        }

    }
    for(int i = 0;i<faces.size();i++)
    {
        float vetex[3]={0,0,0};
        for(int j = 0; j < 3;j++)
        {
            for(int k = 0; k < 3;k++)
            {
                vetex[k]=positions[faces[i].v[j]].v[k];
            }
            binaryply.setComponent(vetex,j);
        }
        m_plybinary.push_back(binaryply);
    }
    qDebug()<<"number of ASCII faces"<<m_plybinary.size()<<endl;
    return true;
}

bool PLYData::parseBinary()
{
    QByteArray filename = m_fileName.toLocal8Bit();
    ifstream plyASCIIStream(filename.data(),ios::in|ios::binary);
    size_t headByte=0;
    int bufflen = 256;
    char buff[256];
    string line;
    stringstream strStream;
    BinaryPLY binaryply;
    if(!plyASCIIStream.is_open())
    {
        return false;
    }
    while(!plyASCIIStream.eof())
    {
        memset(buff,0,bufflen);
        plyASCIIStream.getline(buff,256);
        strStream.str(buff);
        headByte +=strlen(buff);
        line=buff;
        //cout<<line<<endl;
        if(line.find("end_header")==0)
        {
            headByte +=strlen(buff);
            //cout<<"headByte:"<<headByte<<"Byte:"<<strlen(buff)<<endl;
            break;
        }
        //cout<<"headByte:"<<headByte<<"Byte:"<<strlen(buff)<<endl;
    }

    ifstream plyDataStream(filename.data(),ios::in|ios::binary);

    if(!plyDataStream.is_open())
    {
        return false;
    }
    plyDataStream.seekg(0,ios::end);// 定位到文件结束处
    double length =  plyDataStream.tellg();//文件大小
    length -=headByte;
    float *positions=new float[m_vertexNumb*3];
    int *faces=new  int[100];
    int num=0;
    //qDebug()<<"size of file is  "<<length<<endl;
    plyDataStream.clear();
    plyDataStream.seekg(headByte-1,ios::beg);//输入流的开始
    plyDataStream.read(reinterpret_cast<char*>(&positions[0]),m_vertexNumb*3*sizeof(positions[0]));
    for(int i=0;i<m_faceNumb;i++)
    {

        plyDataStream.read((char *)&num,1);        
        plyDataStream.read(reinterpret_cast<char*>(&faces[0]),num*sizeof(faces[0]));
        for(int i=0;i<num-2;i++)
        {
            Vec3D vertex0(positions[faces[0]*3],positions[faces[0]*3+1],positions[faces[0]*3+2]);
            Vec3D vertex1(positions[faces[1]*3],positions[faces[1]*3+1],positions[faces[1]*3+2]);
            Vec3D vertex2(positions[faces[2]*3],positions[faces[2]*3+1],positions[faces[2]*3+2]);
            binaryply.setComponent(vertex0,0);
            binaryply.setComponent(vertex1,1);
            binaryply.setComponent(vertex2,2);
            m_plybinary.push_back(binaryply);
        }
    }
    plyDataStream.close();
    delete []faces;
    //m_plybinary[0].printMe();
    qDebug()<<"number of binary faces"<<m_plybinary.size()<<endl;
    return true;
}

void PLYData::finishAddTriangles()
{
    mesh->finishAddingVertices(vertexHasher);
    mesh->finishAddingFaces(edgeHasher);
    //mesh->updateVertexNormals();
    mesh->calFacesZ();
}

void PLYData::ply2Mesh(DataRP::TriangleMesh *triMesh)
{
    mesh = triMesh;
    Vec3D vec[3];
    PLYBinary::iterator faceIt;
    DataRP::PolygonMesh::VertexIterator vertexIts[3];

    DataRP::TriangleMesh::VertexHasher::Iterator verHashIt;
    vertexHasher = mesh->startAddingVertex();
    edgeHasher = mesh->startAddingFaces();

    double minmax[3][2]; //minX,maxX  minY,maxY, minZ,maxZ
    m_plybinary[0].getVertices(vec);
    for(int i = 0; i < 2; i++)
    {
        minmax[0][i] = vec[i].getX();
        minmax[1][i] = vec[i].getY();
        minmax[2][i] = vec[i].getZ();
    }


    for(faceIt = m_plybinary.begin(); faceIt != m_plybinary.end(); faceIt++)
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

RPIOply::RPIOply(DataRP::RPData *pdata)
{
    rp_data = pdata;
}

bool RPIOply::readPLYfile(QString plyfileName)
{
    PLYhead plyheader(plyfileName);
    DataRP::TriangleMesh *plymesh= rp_data->createNewMesh();

    if(!plyheader.parsePLY())
    {
       cout<<"cannot read ply head"<<endl;
       return false;
    }

    float m_faceNumb = plyheader.getFaceNumb();
    cout<<"face number is  "<<m_faceNumb<<endl;
    PLYData plydata(plyheader);
    if(!plydata.parsePLY())
    {
       cout<<"cannot read ply data"<<endl;
       return false;
    }
    plydata.ply2Mesh(plymesh);
    rp_data->refresh();
    return true;
}

