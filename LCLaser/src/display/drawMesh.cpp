/***************************************************************************
                             drawMesh.cpp  -  description
                             -------------------
    begin                : Jun. 3 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "drawMesh.h"

using namespace RenderRP;


RPDrawMesh::RPDrawMesh()
{
}

RPDrawMesh::RPDrawMesh(DataRP::RPData *rpdata):RPdraw(rpdata)
{
}

RPDrawMesh::~RPDrawMesh()
{
}

void RPDrawMesh::draw()
{
    if(b_wireframe)
    {
        drawWireFrame();
    }
    else
    {
        drawSolid();
        //drawNormals();
    }
}

void RPDrawMesh::drawSolid()
{
    DataRP::RPVertices vertices;
    DataRP::RPVertices edges;
    DataRP::RPVertices normals;
    Vec3D point,v1,v2;

    int facenumb = 0;
    unsigned numbpoints = 0;
    DataRP::TriangleMesh *smesh = rp_data->getMesh();
    for(DataRP::TriangleMesh::ConstFaceIterator fIt = smesh->beginFaces(); fIt != smesh->endFaces(); ++fIt)
       {
           for(DataRP::TriangleMesh::ConstFaceEdgeIterator feIt = fIt.beginEdges(); feIt != fIt.endEdges(); ++feIt)
           {
               point = feIt->getStart()->normal;
               normals.push_back(point);
               point = (*feIt->getStart());
               vertices.push_back(point);

               v1 = fIt->getNormal();
               if(feIt.isUpperHalf())
               {
                   if(feIt->getOpposite()==0)
                   {
                       point = (*feIt->getStart());
                       edges.push_back(point);
                       point = (*feIt->getEnd());
                       edges.push_back(point);
                   }
                   else
                   {
                        v2 = feIt->getOpposite()->getFace()->getNormal();
                        double dot = v1.Dot(v2);
                        if(dot < 0)
                            dot = -dot;//dot < 0.707
                        if((dot  < 0.707 ))//cos(PI/4)
                        {
                            point = (*feIt->getStart());
                            edges.push_back(point);
                            point = (*feIt->getEnd());
                            edges.push_back(point);
                        }
                   }//else
               }//upperhalf.
           }
           facenumb++;
       }

    glPushAttrib(GL_CURRENT_BIT);

    //glDisable(GL_LIGHTING);
    glClear(GL_DEPTH_BUFFER_BIT);

    glLineWidth(2.0f);
    float linecolor[4] = {1.0f,1.0f,1.0f,0.5f};
    glColor4fv(linecolor);
    numbpoints = edges.size();
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_DOUBLE,0,edges.data());
        glDrawArrays(GL_LINES,0,numbpoints);
    glDisableClientState(GL_VERTEX_ARRAY);
    glLineWidth(1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //float scolor[4] = {0.117f,0.565f,1.0f,0.0f};
    float scolor[4] = {0.0f,0.35f,0.67f,0.5f};
    glColor4fv(scolor);
    numbpoints = vertices.size();
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_DOUBLE,0,normals.data());
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_DOUBLE,0,vertices.data());
            glDrawArrays(GL_TRIANGLES,0,numbpoints);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glDisable(GL_BLEND);
    //glEnable(GL_LIGHTING);


    glPopAttrib();
}

void RPDrawMesh::drawWireFrame()
{

    //Vec3D v1,v2;
    DataRP::TriangleMesh *smesh = rp_data->getMesh();
     /* Render all mesh edges: */
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glBegin(GL_LINES);

    for(DataRP::TriangleMesh::ConstFaceIterator fIt=smesh->beginFaces();fIt!=smesh->endFaces();++fIt)
        for(DataRP::TriangleMesh::ConstFaceEdgeIterator feIt=fIt.beginEdges();feIt!=fIt.endEdges();++feIt)
        {
            if(feIt.isUpperHalf())
            {
                if(feIt->getOpposite()==0)
                {
                    glColor3f(0.0f,0.5f,1.0f);
                    glVertex3d(feIt->getStart()->getX(),feIt->getStart()->getY(),feIt->getStart()->getZ());
                    glVertex3d(feIt->getEnd()->getX(),feIt->getEnd()->getY(),feIt->getEnd()->getZ());
                }
                else
                {
//                    v1 = fIt->getNormal();
//                    v2 = feIt->getOpposite()->getFace()->getNormal();
//                    double dot = v1.Dot(v2);
//                    if(dot < 0)
//                    dot = -dot;
//                    if(dot < 0.707)
                    //{
                     glColor3f(0.2f,0.2f,0.2f);
                     glVertex3d(feIt->getStart()->getX(),feIt->getStart()->getY(),feIt->getStart()->getZ());
                     glVertex3d(feIt->getEnd()->getX(),feIt->getEnd()->getY(),feIt->getEnd()->getZ());
                    //}
                }
             }
        }
        glEnd();
        glLineWidth(1.0f);
        glEnable(GL_LIGHTING);
}


void RPDrawMesh::drawNormals()
{
    Vec3D normal,center,cend,point[3];
    int index = 0;
    DataRP::TriangleMesh *smesh = rp_data->getMesh();

    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for(DataRP::TriangleMesh::ConstFaceIterator fIt = smesh->beginFaces(); fIt != smesh->endFaces(); ++fIt)
    {
        normal = fIt->getNormal();
        index = 0;
        for(DataRP::TriangleMesh::ConstFaceEdgeIterator feIt = fIt.beginEdges(); feIt != fIt.endEdges(); ++feIt)
        {
            point[index] = *feIt->getStart();
            index++;
            if(index > 3)
                return;
        }
        center.setX((point[0].getX() + point[1].getX() + point[2].getX())/3);
        center.setY((point[0].getY() + point[1].getY() + point[2].getY())/3);
        center.setZ((point[0].getZ() + point[1].getZ() + point[2].getZ())/3);
        cend = center +normal*1.5;
        glVertex3d(center.getX(), center.getY(), center.getZ());
        glVertex3d(cend.getX(), cend.getY(), cend.getZ());
    }
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}




