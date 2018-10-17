/***************************************************************************
                          graph.cpp  -  description
                             -------------------
    begin                : May 2 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#include "graph.h"

using namespace RenderRP;
//!
//! \brief draw laser head model
//! \param height  height of laser head.
//! \param the height of light cone when laser is turn on.
//! \param width  width of laser head.
//! \param slices  how many slices of cylinder or disk.
//! \par1am isopen is laser opened.
//!
void Graphs::modelLaserHead(double height,double coneface,double width,double slices,bool isopen)
{
    if(height < coneface)
        return;

    float hcl[3][3];

    if(isopen)
    {
        hcl[0][0] = 1.0f;  hcl[0][1] = 0.0f;  hcl[0][2] = 0.0f;
        hcl[1][0] = 0.85f; hcl[1][1] = 0.70f; hcl[1][2] = 0.45f;
        hcl[2][0] = 0.8f;  hcl[2][1] = 0.0f;  hcl[2][2] = 0.0f;
    }
    else
    {
        hcl[0][0] = 0.86f;  hcl[0][1] = 0.87f;  hcl[0][2] = 0.89f;
        hcl[1][0] = 0.86f*0.7f;  hcl[1][1] = 0.87f*0.7f;  hcl[1][2] = 0.89f*0.7f;
        hcl[2][0] = 0.86f;  hcl[2][1] = 0.90f;  hcl[2][2] = 0.94f;


    }

    GLUquadricObj *qobj = gluNewQuadric();
    if(isopen)
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(hcl[0][0],hcl[0][1],hcl[0][2],0.3f);
        //obj, baseRadius, topRadius, height,slices, stacks
        gluCylinder(qobj, 0, width, coneface, slices, 1);
        glDisable(GL_BLEND);
    }


    glColor3f(hcl[1][0],hcl[1][1],hcl[1][2]);
    glTranslatef(0.0, 0.0, coneface);
    gluCylinder(qobj, width, width, height-coneface, slices, 1);

    glColor3f(hcl[2][0],hcl[2][1],hcl[2][2]);
    gluQuadricOrientation(qobj, GLU_OUTSIDE);
    glTranslatef(0.0, 0.0, height-coneface);
    gluDisk(qobj, 0.0, width, slices, 1);

    glTranslatef(0.0, 0.0, -height);
    glColor3f(0,0,0);
}

void Graphs::modleTrack(Vec3D tstart,Vec3D tend,double radius)
{
    double length;
    Vec3D leftbot,rightbot,lefttop,righttop;

    Vec3D normal = tend - tstart;
    length = normal.Length();
    normal.Normalize();
    Vec3D right = normal.VerticalXYRight();
    Vec3D left = normal.VerticalXYLeft();

    FilledArc(tend,right,radius,12);

    if(length < radius)
        return;

    FilledArc(tstart,left,radius,12);

    leftbot = left  * radius + tstart;
    rightbot= right * radius + tstart;
    lefttop = left  * radius + tend;
    righttop= right * radius + tend;


    glTranslatef(0,0,tend.getZ());
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2d(lefttop.getX(),lefttop.getY());
        glVertex2d(leftbot.getX(),leftbot.getY());
        glVertex2d(tend.getX(),tend.getY());
        glVertex2d(tstart.getX(),tstart.getY());
        glVertex2d(righttop.getX(),righttop.getY());
        glVertex2d(rightbot.getX(),rightbot.getY());
    glEnd();
    glTranslatef(0,0,-tend.getZ());

}

void Graphs::drawTrackHead(Line &line,double radius)
{
    Vec3D normal = line.getVector();
    Vec3D thead = line.getHead();

    Vec3D left = normal.VerticalXYLeft();
    FilledArc(thead,left,radius,12);
}

void Graphs::drawTrackEnd(Line &line,double radius)
{
    Vec3D normal = line.getVector();
    Vec3D tail = line.getTail();

    Vec3D right = normal.VerticalXYRight();
    FilledArc(tail,right,radius,12);
}

void Graphs::drawTrackTerminal(Vec3D scenter,Vec3D svec, double sradius,double slices)
{
    FilledArc(scenter,svec,sradius,slices);
}

void Graphs::drawTrackBody(Vec3D leftstart,Vec3D rightstart,Vec3D leftend,Vec3D rightend)
{
    glTranslatef(0,0,rightend.getZ());
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2d(leftstart.getX(),leftstart.getY());
        glVertex2d(rightstart.getX(),rightstart.getY());
        glVertex2d(leftend.getX(),leftend.getY());
        glVertex2d(rightend.getX(),rightend.getY());
    glEnd();
    glTranslatef(0,0,-rightend.getZ());

}

void Graphs::FilledArc(Vec3D center,Vec3D arm,double radius, double slices)
 {
    double radian = M_PI;
    double step,cur;
    Vec3D target;
    Vec3D vrot;
    Vec3D vzero = Vec3D(0.0,0.0,0.0);

    if(arm == vzero)
        return;

    glBegin(GL_TRIANGLE_FAN);
//    glPointSize(4.0f);
//    glBegin(GL_POINTS);
    glVertex3d(center.getX(),center.getY(),center.getZ());
    for (int i = 0; i < slices; i++)
    {
        vrot = arm;
        step = (double) i / (slices - 1);
        cur = step * radian;
        vrot.RotZ(cur);
        target = vrot * radius + center;
        glVertex3d(target.getX(),target.getY(),target.getZ());
    }
    glEnd();
    //glPointSize(1.0f);
 }

void Graphs::drawElbow(Vec3D start,Vec3D send,Vec3D interPoint,Vec3D center, double sradius,short dir)
{
    //glColor4f(1.0f, 1.0f, 0.0f, 0.5f);

    double cosine,radian,step,angle = 0;
    double dist = 0.1;
    Vec3D vstart = start - center;
    Vec3D vend = send - center;
    Vec3D vrot,target;

    if(dir !=1 && dir !=-1)
    {
       qDebug()<<"no direction"<<endl;
       return;
    }
    vstart.Normalize();
    vend.Normalize();

    if(dir==1)
       cosine = vstart.Dot(vend);
    else
       cosine = vend.Dot(vstart);

    radian = acos(cosine);
    step = dist/(M_PI*sradius);
    angle = step;

    glBegin(GL_TRIANGLE_FAN);
        //draw center.
        glVertex3d(interPoint.getX(),interPoint.getY(),interPoint.getZ());
        //first
        if(dir ==1)
            glVertex3d(start.getX(),start.getY(),start.getZ());
        else
             glVertex3d(send.getX(),send.getY(),send.getZ());

        while(angle < radian)
        {
            if(dir == 1)
            {
                 vrot = vstart;
            }
            else
            {
                 vrot = vend;
            }

            vrot.RotZ(angle);

            target = vrot * sradius + center;
            glVertex3d(target.getX(),target.getY(),target.getZ());
            angle += step;
        }

        if(dir ==1)
            glVertex3d(send.getX(),send.getY(),send.getZ());
        else
            glVertex3d(start.getX(),start.getY(),start.getZ());

    glEnd();
    //glColor4f(0.0f, 1.0f, 1.0f, 0.5f);

}

void Graphs::drawArc(Vec3D start,Vec3D center,Vec3D send,double radian,double sradius,double trackWidth)
{

    if(radian == 0)
      return;

    glPointSize(4.0);
    glBegin(GL_POINTS);
        glVertex3f(center.getX(),center.getY(),center.getZ());
    glPointSize(1.0);
    glEnd();

    glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
    double step =0 ,angle = 0, diff = 1;
    double dist = 1.0;
    Vec3D vstart = start - center;
    Vec3D vtop,vbot;
    Vec3D vrot,target;
    DataRP::RPVertices vertices;
    DataRP::RPVertices vertars;
    unsigned numbpoints;

     if(sradius == -1)
         sradius = -sradius;

    vstart.Normalize();
    step = dist/(M_PI*sradius);
    if(radian < 0)
        step = -step;
    angle = step;

    target = start;
    vrot = vstart;
    vtop = vrot*trackWidth + target;
    vbot = target - vrot*trackWidth;

    if(radian < 0)
    {
        vertices.push_back(vtop);
        vertices.push_back(vbot);
    }
    else
    {
        vertices.push_back(vbot);
        vertices.push_back(vtop);
    }
    vertars.push_back(target);

    while(diff > 0)
    {
        vrot = vstart;
        vrot.RotZ(angle);
        target = vrot * sradius + center;

        vtop = vrot*trackWidth + target;
        vbot = target - vrot*trackWidth;

        if(radian < 0)
        {
            vertices.push_back(vtop);
            vertices.push_back(vbot);
        }
        else
        {
            vertices.push_back(vbot);
            vertices.push_back(vtop);
        }
        vertars.push_back(target);

        angle += step;
        diff = radian - angle;
        if(radian < 0)
            diff = -diff;
    }//while

    target = send;
    vrot = send - center;
    vrot.Normalize();
    vtop = vrot*trackWidth + target;
    vbot = target - vrot*trackWidth;
    if(radian < 0)
    {
        vertices.push_back(vtop);
        vertices.push_back(vbot);
    }
    else
    {
        vertices.push_back(vbot);
        vertices.push_back(vtop);
    }
    vertars.push_back(target);

    numbpoints = vertices.size();
    glEnableClientState(GL_VERTEX_ARRAY);
          glVertexPointer(3,GL_DOUBLE,0,vertices.data());
          glDrawArrays(GL_TRIANGLE_STRIP,0,numbpoints);
    glDisableClientState(GL_VERTEX_ARRAY);

//    numbpoints = vertars.size();
//    glEnableClientState(GL_VERTEX_ARRAY);
//          glVertexPointer(3,GL_DOUBLE,0,vertars.data());
//          glDrawArrays(GL_LINE_STRIP,0,numbpoints);
//    glDisableClientState(GL_VERTEX_ARRAY);


    glColor4f(0.0f, 1.0f, 1.0f, 0.5f);
}

void Graphs::drawAnimArc(Vec3D start,Vec3D center, double radian, double sradius, double trackWidth)
{
    if(radian == 0)
      return;

    glPointSize(10.0);
    glBegin(GL_POINTS);
        glVertex3f(center.getX(),center.getY(),center.getZ());
    glPointSize(1.0);
    glEnd();

    double step =0 ,angle = 0, diff = 1;
    double dist = 1.0;
    Vec3D vstart = start - center;
    Vec3D vtop,vbot;
    Vec3D vrot,target;
    DataRP::RPVertices vertices;
    unsigned numbpoints;

     if(sradius == -1)
         sradius = -sradius;

    vstart.Normalize();
    step = dist/(M_PI*sradius);
    if(radian < 0)
        step = -step;
    angle = step;

    target = start;
    vrot = vstart;
    vtop = vrot*trackWidth + target;
    vbot = target - vrot*trackWidth;

    if(radian < 0)
    {
        vertices.push_back(vtop);
        vertices.push_back(vbot);
    }
    else
    {
        vertices.push_back(vbot);
        vertices.push_back(vtop);
    }

    while(diff > 0)
    {
        diff = radian - angle;
        if(radian < 0)
            diff = -diff;

        vrot = vstart;
        vrot.RotZ(angle);
        target = vrot * sradius + center;

        vtop = vrot*trackWidth + target;
        vbot = target - vrot*trackWidth;

        if(radian < 0)
        {
            vertices.push_back(vtop);
            vertices.push_back(vbot);
        }
        else
        {
            vertices.push_back(vbot);
            vertices.push_back(vtop);
        }
        angle += step;
    }//while

    numbpoints = vertices.size();
    glEnableClientState(GL_VERTEX_ARRAY);
          glVertexPointer(3,GL_DOUBLE,0,vertices.data());
          glDrawArrays(GL_TRIANGLE_STRIP,0,numbpoints);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Graphs::modelCube()
{
    glBegin(GL_QUADS);				// start drawing the cube.

    // top of cube
    glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Green
    glVertex3f( 1.0f, 1.0f, 1.0f);		// Top Right Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f, 1.0f);		// Top Left Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f, 3.0f);		// Bottom Left Of The Quad (Top)
    glVertex3f( 1.0f, 1.0f, 3.0f);		// Bottom Right Of The Quad (Top)

    // bottom of cube
    glColor3f(1.0f,0.5f,0.0f);			// Set The Color To Orange
    glVertex3f( 1.0f,-1.0f, 3.0f);		// Top Right Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f, 3.0f);		// Top Left Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f, 1.0f);		// Bottom Left Of The Quad (Bottom)
    glVertex3f( 1.0f,-1.0f, 1.0f);		// Bottom Right Of The Quad (Bottom)

    // front of cube
    glColor3f(1.0f,1.0f,0.0f);			// Set The Color To YELLOW
    glVertex3f( 1.0f, 1.0f, 3.0f);		// Top Right Of The Quad (Front)
    glVertex3f(-1.0f, 1.0f, 3.0f);		// Top Left Of The Quad (Front)
    glVertex3f(-1.0f,-1.0f, 3.0f);		// Bottom Left Of The Quad (Front)
    glVertex3f( 1.0f,-1.0f, 3.0f);		// Bottom Right Of The Quad (Front)

    // back of cube.
    glColor3f(1.0f,0.0f,0.0f);			// Set The Color To RED
    glVertex3f( 1.0f,-1.0f, 1.0f);		// Top Right Of The Quad (Back)
    glVertex3f(-1.0f,-1.0f, 1.0f);		// Top Left Of The Quad (Back)
    glVertex3f(-1.0f, 1.0f, 1.0f);		// Bottom Left Of The Quad (Back)
    glVertex3f( 1.0f, 1.0f, 1.0f);		// Bottom Right Of The Quad (Back)

    // left of cube
    glColor3f(0.0f,0.0f,1.0f);			// Blue
    glVertex3f(-1.0f, 1.0f, 3.0f);		// Top Right Of The Quad (Left)
    glVertex3f(-1.0f, 1.0f, 1.0f);		// Top Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f, 1.0f);		// Bottom Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f, 3.0f);		// Bottom Right Of The Quad (Left)

    // Right of cube
    glColor3f(1.0f,0.0f,1.0f);			// Set The Color To Violet
    glVertex3f( 1.0f, 1.0f, 1.0f);	        // Top Right Of The Quad (Right)
    glVertex3f( 1.0f, 1.0f, 3.0f);		// Top Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f, 3.0f);		// Bottom Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f, 1.0f);		// Bottom Right Of The Quad (Right)
    glEnd();					// Done Drawing The Cubed
}

void Graphs::drawGrid(Grid &grid)
{
    //unsigned step_x,step_y;
    GLfloat min_x, min_y, max_x, max_y;
    max_x = grid.grid_width/2;
    min_x = - max_x;
    max_y = grid.grid_height/2;
    min_y = - max_y;
    GLfloat hz = 0;

    GLfloat Bmin_x, Bmin_y, Bmax_x, Bmax_y;
    Bmin_x = min_x;  Bmin_y = min_y;  Bmax_x = max_x;  Bmax_y = max_y;

    if(floor((float)grid.grid_width / grid.grid_scale) < 2)
    {
        return;
    }

    if(grid.grid_width % grid.grid_scale!= 0 )
    {

        max_x = (grid.grid_scale * floor((float)grid.grid_width / grid.grid_scale))/2;
        min_x = -max_x;
    }
    //qDebug()<<"max_x =  "<<max_x<<endl;

    if(grid.grid_height % grid.grid_scale!= 0 )
    {
        max_y = (grid.grid_scale * floor((float)grid.grid_height / grid.grid_scale))/2;
        min_y = -max_y;
    }

    glBegin(GL_LINES);
    glColor3f(1.0f,1.0f,1.0f);
    //glColor3f(0.0f,0.0f,0.0f);

    //vertical lines.
    for(GLfloat line_x = min_x ; line_x <= max_x;  line_x+= grid.grid_scale)
    {
         glVertex3f(line_x,min_y,hz);
         glVertex3f(line_x,max_y,hz);
    }

    //horizon lines.
    for(GLfloat line_y = min_y ; line_y <= max_y;  line_y+= grid.grid_scale)
    {
         glVertex3f(min_x,line_y,hz);
         glVertex3f(max_x,line_y,hz);
    }

     glEnd();

     glLineWidth(2.0f);
     glBegin(GL_LINE_STRIP);				// start drawing the Bounding.
         glColor3f(0.0f,1.0f,0.0f);
         glVertex3f(Bmax_x,Bmax_y,hz);
         glVertex3f(Bmin_x,Bmax_y,hz);
         glVertex3f(Bmin_x,Bmin_y,hz);
         glVertex3f(Bmax_x,Bmin_y,hz);
         glVertex3f(Bmax_x,Bmax_y,hz);
     glEnd();
      glLineWidth(1.0f);

}

void Graphs::drawLines(DataRP::RPVertices &lines,float linewidth, DataRP::RPColor color,bool bstrip)
{
    unsigned numbpoints = lines.size();

    glPushAttrib(GL_CURRENT_COLOR | GL_LINE_WIDTH);
    glLineWidth(linewidth);
    glColor3f(color.red(),color.green(),color.blue());

    glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3,GL_DOUBLE,0,lines.data());

      if(bstrip)
         glDrawArrays(GL_LINE_STRIP,0,numbpoints);
      else
         glDrawArrays(GL_LINES,0,numbpoints);

      glDisableClientState(GL_VERTEX_ARRAY);
    glPopAttrib();
}

void Graphs::drawLines(DataRP::Tracks &lines,float linewidth,DataRP::RPColor color,float scale, bool showArrow)
{

if(lines.size() < 1)
    return;

    unsigned numbpoints = 0;
    DataRP::RPVertices points;
    Vec3D lhead,ltail;
    DataRP::Tracks::iterator tit;

for(tit = lines.begin(); tit != lines.end(); tit++)
{
    lhead = tit->getHead();
    ltail = tit->getTail();
    points.push_back(lhead);
    points.push_back(ltail);
}
numbpoints = points.size();

    glLineWidth(linewidth);
    glColor3f(color.red(),color.green(),color.blue());
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_DOUBLE,0,points.data());

    glDrawArrays(GL_LINES,0,numbpoints);

    glDisableClientState(GL_VERTEX_ARRAY);
    glColor3f(0,0,0);
    glLineWidth(1);

    if(showArrow)
        drawArrows(points,scale,false,true);
}


void Graphs::drawArrows(DataRP::RPVertices &lines,float scale,bool bstrip, bool onhead)
{
    if(lines.size() < 2)
        return;

    DataRP::RPVertices::iterator itline;
    DataRP::RPVertices arrows;
    Vec3D vect[5];
    double vz,deltaArraw;
    double s_scale = 20;

    double delta = 1;

    if(scale > s_scale*2)
    {
        deltaArraw = 0.8;
    }
    else
    {
         deltaArraw = scale/50; //5.5
    }


    unsigned numbpoints = 0;

    for(itline = lines.begin()+1; itline != lines.end(); itline++)
    {
         vect[0] = *itline;    //head
         vect[4] = *(itline-1);//trail

         vz = itline->getZ();
         vect[0].setZ(0.0);
         vect[4].setZ(0.0);

         vect[3] = vect[0] - vect[4];//line

         if(!onhead)
            vect[0] = vect[4] + vect[3]*0.8; //head

         vect[3].Normalize(); //line
         vect[3] *= delta;

         //vertical vectors
         vect[3].VerticalXY(vect[1],vect[2]);
         vect[1].RotZ(0.5235988);
         vect[2].RotZ(-0.5235988);
         //arrow vectors
         vect[1] = vect[3] - vect[1];
         vect[2] = vect[3] - vect[2];
         //arrow length
         vect[1] *= deltaArraw;
         vect[2] *= deltaArraw;
         //arrow points
         vect[1] = ( vect[0] - vect[1]);
         vect[2] = ( vect[0] - vect[2]);
         vect[3] = vect[0];

         for(int i = 0; i < 4; i++)
         {
             vect[i].setZ(vz);
             arrows.push_back(vect[i]);
         }
        numbpoints += 4;

        if(!bstrip)
        {
            itline++;

            if(itline == lines.end())
            break;
        }
   }

    glColor3f(1.0f,0.3f,1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3,GL_DOUBLE,0,arrows.data());
      glDrawArrays(GL_LINES,0,numbpoints);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Graphs::drawArrows(DataRP::RPVertices &lines,double vz, float scale, bool onhead)
{

    if(lines.size() < 2)
        return;

    DataRP::RPVertices::iterator itline;
    DataRP::RPVertices arrows;
    Vec3D vect[5];

    double deltaArraw;
    double s_scale = 20;
    unsigned numbpoints = 0;

    if(scale > s_scale)
    {
        deltaArraw = 1.2;
    }
    else
    {
        deltaArraw = scale/50;//2.5;//size and angle
    }

    for(itline = lines.begin()+1; itline != lines.end(); itline++)
    {
         vect[0] = *itline;    //head
         vect[4] = *(itline-1);//trail

         vect[3] = vect[0] - vect[4];//line

         if(!onhead)
            vect[0] = vect[4] + vect[3]/2; //head

         vect[3].Normalize(); //line

         //vertical vectors
         vect[3].VerticalXY(vect[1],vect[2]);
         vect[1].RotZ(0.5235988);
         vect[2].RotZ(-0.5235988);
         //arrow vectors
         vect[1] = vect[3] - vect[1];
         vect[2] = vect[3] - vect[2];
         //arrow length
         vect[1] *= deltaArraw;
         vect[2] *= deltaArraw;
         //arrow points
         vect[1] = ( vect[0] - vect[1]);
         vect[2] = ( vect[0] - vect[2]);
         vect[3] = vect[0];

         for(int i = 0; i < 4; i++)
         {
             vect[i].setZ(vz);
             arrows.push_back(vect[i]);
         }
        numbpoints += 4;
    }//arrow

    glColor3f(1.0f,1.0f,0.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3,GL_DOUBLE,0,arrows.data());
      glDrawArrays(GL_LINES,0,numbpoints);
    glDisableClientState(GL_VERTEX_ARRAY);

}

void Graphs::drawArrow(const Line &line, DataRP::RPColor color)
{
    unsigned numbpoints = 6;
    Vec3D vect[6];
    DataRP::RPVertices arrows;
    double deltaArraw = line.getLength()/3;

    vect[0] = line.getTail();    //arrow head/ linew tail.
    vect[4] = vect[0];
    vect[5] = line.getHead();    //

    vect[3] = vect[0] - vect[5];//line
    vect[3].Normalize(); //line

    //vertical vectors
    vect[3].VerticalXY(vect[1],vect[2]);
    vect[1].RotZ(0.5235988);
    vect[2].RotZ(-0.5235988);
    //arrow vectors
    vect[1] = vect[3] - vect[1];
    vect[2] = vect[3] - vect[2];
    //arrow length
    vect[1] *= deltaArraw;
    vect[2] *= deltaArraw;
    //arrow points
    vect[1] = ( vect[0] - vect[1]);
    vect[2] = ( vect[0] - vect[2]);
    vect[3] = vect[0];

    for(int i = 0; i < 6; i++)
     {
         arrows.push_back(vect[i]);
     }

    glLineWidth(3);
    glColor3f(color.red(),color.green(),color.blue());

     glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3,GL_DOUBLE,0,arrows.data());
      glDrawArrays(GL_LINES,0,numbpoints);
    glDisableClientState(GL_VERTEX_ARRAY);

    glLineWidth(1);
    glColor3f(0.0f, 0.0f, 0.0f);


}

void Graphs::drawDisk(const Vec3D &center,double radius, double slices,DataRP::RPColor color)
{
    GLUquadricObj *qobj = gluNewQuadric();
    gluQuadricOrientation(qobj, GLU_OUTSIDE);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glColor4f(color.red(),color.green(),color.blue(),0.3f);
    glTranslatef(center.getX(),center.getY(),center.getZ());

        gluDisk(qobj, 0.0, radius, slices, 1);

    glTranslatef(-center.getX(),-center.getY(),-center.getZ());
    glColor3f(0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);

}

void Graphs::draw2DFrameBar(DataRP::RPpolygon &vertices)
{
    glColor4f(0.0f,1.0f,1.0f,0.5f);
    unsigned numbpoints = vertices.size();
    glVertexPointer(2,GL_DOUBLE,0,vertices.data());

    glEnableClientState(GL_VERTEX_ARRAY);
      glDrawArrays(GL_LINE_LOOP,0,numbpoints);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Graphs::draw2DBar(DataRP::RPpolygon &vertices)
{
    glColor4f(0.0f,1.0f,1.0f,0.5f);
    unsigned numbpoints = vertices.size();
    glVertexPointer(2,GL_DOUBLE,0,vertices.data());

    glEnableClientState(GL_VERTEX_ARRAY);
      glDrawArrays(GL_QUADS,0,numbpoints);
    glDisableClientState(GL_VERTEX_ARRAY);
}
