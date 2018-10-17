/***************************************************************************
                          matrix4x4.cpp  -  description
                             -------------------
    begin                : oct. 26 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/

#include "matrix4x4.h"

Matrix4X4::Matrix4X4()
{
    loadIdentity();
}

Matrix4X4::Matrix4X4(const Vec4D (&vec)[4])
{
    setValue(vec);
}

Matrix4X4::~Matrix4X4()
{

}

void Matrix4X4::loadIdentity()
{
    Vec4D vec[4];
    vec[0].setValue(1,0,0,0);
    vec[1].setValue(0,1,0,0);
    vec[2].setValue(0,0,1,0);
    vec[3].setValue(0,0,0,1);
    setValue(vec);
}

/***************operator ****************/
Matrix4X4& Matrix4X4::operator=(const Matrix4X4 &other)
{
    for(int i = 0; i < 4; i++)
    {
        component[i] = other.component[i];
    }
    return *this;
}

Vec4D Matrix4X4::operator*(Vec4D vec)
{
    double val[4] = {0,0,0,0};
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            val[i] += component[i].component[j]*vec.component[j];
        }
    }
    Vec4D resultVec = Vec4D(val);
    return resultVec;
}

Vec3D Matrix4X4::operator*(Vec3D vec)
{
    Vec4D vec4r,vec4;
    vec4 = vec.toVec4D();

    Matrix4X4 mat = *this;
    vec4r = mat*vec4;
    return vec4r.toVec3D();
}

Matrix4X4 Matrix4X4::operator*(Matrix4X4 smat)
{
    Vec4D tarv[4];
    Vec4D vec[4];
    smat.getColumns(vec);
    Matrix4X4 matself = *this;
    for(int i = 0; i < 4; i++)
    {
        tarv[i] = matself*vec[i];
    }

    Matrix4X4 resultMat(tarv);
    resultMat.trim();
    return resultMat.transpose();
}

/***************matrix calculate ****************/
Matrix4X4 Matrix4X4::inverse()
{
    int n = 4;
    int i,j,k;
    int *is, *js;
    double *p;
    double temp,fmax;
    double a[4][4];
    Matrix4X4 resultMat;

    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            a[i][j] = component[i].getValue(j);
        }
    }

    p = a[0];
    is=(int *)malloc(n*sizeof(int));
    js=(int *)malloc(n*sizeof(int));

    for(k=0;k<n;k++)
    {
        fmax=0.0;
        for(i=k;i<n;i++)
        {
            for(j=k;j<n;j++)
            { temp=fabs(*(p+i*n+j));//find max value;
              if(temp>fmax)
               { fmax=temp;
                 is[k]=i;js[k]=j;
                }
            }
        }

        if((fmax+1.0)==1.0)
        {  free(is);free(js);
           cout<<"no inv"<<endl;
           return resultMat;
        }

        if((i=is[k])!=k)
        {
            for(j=0;j<n;j++)
              swap2((p+k*n+j),(p+i*n+j));
        }

        if((j=js[k])!=k)
        {
            for(i=0;i<n;i++)
              swap2((p+i*n+k),(p+i*n+j));
        }

        p[k*n+k]=1.0/p[k*n+k];
        for(j=0;j<n;j++)
        {
            if(j!=k)
                p[k*n+j]*=p[k*n+k];
        }

        for(i=0;i<n;i++)
        {
            if(i!=k)
            {
               for(j=0;j<n;j++)
               {
                  if(j!=k)
                     p[i*n+j]=p[i*n+j]-p[i*n+k]*p[k*n+j];
               }
            }
        }

        for(i=0;i<n;i++)
        {
            if(i!=k)
               p[i*n+k]*=-p[k*n+k];
        }

 }

 for(k=n-1;k>=0;k--)
 {
     if((j=js[k])!=k)
     {
        for(i=0;i<n;i++)
          swap2((p+j*n+i),(p+k*n+i));
     }

     if((i=is[k])!=k)
     {
         for(j=0;j<n;j++)
          swap2((p+j*n+i),(p+j*n+k));
     }
  }

  free(is);
  free(js);

  Vec4D vec[4];
  for(i = 0; i < 4; i++)
  {
      vec[i].setValue(a[i][0],a[i][1],a[i][2],a[i][3]);
  }
  resultMat.setValue(vec);
  resultMat.trim();
  return resultMat;
}

Matrix4X4 Matrix4X4::rotX(double degreen)
{
    Vec4D vec[4];
    vec[0].setValue(1,0,0,0);
    vec[1].setValue(0,cos(DEG2RAD(degreen)),sin(DEG2RAD(degreen)),0);
    vec[2].setValue(0,-sin(DEG2RAD(degreen)),cos(DEG2RAD(degreen)),0);
    vec[3].setValue(0,0,0,1);
    Matrix4X4 mat(vec);
    Matrix4X4 matself = *this;
    return matself*mat;
}

Matrix4X4 Matrix4X4::rotY(double degreen)
{
    Vec4D vec[4];
    vec[0].setValue(cos(DEG2RAD(degreen)),0,-sin(DEG2RAD(degreen)),0);
    vec[1].setValue(0, 1 ,0 ,0);
    vec[2].setValue(sin(DEG2RAD(degreen)),0,cos(DEG2RAD(degreen)),0);
    vec[3].setValue(0,0,0,1);
    Matrix4X4 mat(vec);
    Matrix4X4 matself = *this;
    return matself*mat;
}

Matrix4X4 Matrix4X4::rotZ(double degreen)
{
    Vec4D vec[4];
    vec[0].setValue(cos(DEG2RAD(degreen)),sin(DEG2RAD(degreen)),0,0);
    vec[1].setValue(-sin(DEG2RAD(degreen)),cos(DEG2RAD(degreen)),0,0);
    vec[2].setValue(0,0,1,0);
    vec[3].setValue(0,0,0,1);
    Matrix4X4 mat(vec);
    Matrix4X4 matself = *this;
    return matself*mat;
}

Matrix4X4 Matrix4X4::translate(double x,double y,double z)
{
    Matrix4X4 mat;
    mat.setTransValue(x,y,z);
    Matrix4X4 matself = *this;
    return matself*mat;
}

Matrix4X4 Matrix4X4::transpose()
{
    Vec4D vec[4];
    double element[4] = {0,0,0,0};
    for(int i = 0 ;i < 4; i++)
    {
        for(int j = 0 ;j < 4; j++)
        {
            element[j] = component[j].component[i];
        }
        vec[i].setValue(element);
    }
    Matrix4X4 mat(vec);
    return mat;
}


/***************set and get ****************/
void Matrix4X4::setValue(const Vec4D (&vec)[4])
{
    for(int i = 0; i < 4; i++)
    {
        Vec4D vect = vec[i];
        component[i] = vect;
    }
}

//index: 11,2,3,4
void Matrix4X4::setValue(int index, const Vec4D &vec)
{
    if(index > 3 || index < 0)
        return;

    component[index] = vec;
}


void Matrix4X4::getRows(Vec4D (&vec)[4])
{
    for(int i = 0; i < 4; i++)
    {
        vec[i].setValue(component[i].component[0],component[i].component[1],component[i].component[2],component[i].component[3]);
    }
}

Vec3D Matrix4X4::getRows(int index)
{
     Vec3D vec = Vec3D(component[index].component[0],component[index].component[1],component[index].component[2]);
     return vec;
}

void Matrix4X4::getColumns(Vec4D (&vec)[4])
{
    for(int i = 0; i < 4; i++)
    {
        vec[i].setValue(component[0].component[i],component[1].component[i],component[2].component[i],component[3].component[i]);
    }
}

void Matrix4X4::getColumns(Vec4D (&vec)[4]) const
{
    for(int i = 0; i < 4; i++)
    {
        vec[i].setValue(component[0].component[i],component[1].component[i],component[2].component[i],component[3].component[i]);
    }
}

void Matrix4X4::setTransValue(double x,double y,double z)
 {
     component[0].setValue(x);
     component[1].setValue(y);
     component[2].setValue(z);
 }

void Matrix4X4::getTransVal(double &tx, double &ty, double &tz)
{
    tx = component[0].getValue(3);
    ty = component[1].getValue(3);
    tz = component[2].getValue(3);
}

void Matrix4X4::swap2(double *a,double *b)
{
    double c;
    c=*a;
    *a=*b;
    *b=c;
}

void Matrix4X4::printMe()
{
    for(int i = 0; i < 4; i++)
    {
            qDebug()<<component[i].component[0]<<"  "<<component[i].component[1]<<"  "<<component[i].component[2]<<"  "<<component[i].component[3]<<endl;
    }
    qDebug()<<endl;
}

void Matrix4X4::printMe(std::stringstream &strstream)
{
    for(int i = 0; i < 4; i++)
    {
            strstream<<component[i].component[0]<<"  "<<component[i].component[1]<<"  "<<component[i].component[2]<<"  "<<component[i].component[3]<<endl;
    }
}

void Matrix4X4::trim()
{
    double coef = pow(10.0, 10.0);
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
        {
            if(component[i].component[j] < 10e-15 && component[i].component[j] > -10e-15)
                component[i].component[j] = 0;

            double tv = floor(component[i].component[j]*coef)/coef;
            if(tv == 1.0)
                component[i].component[j] = 1.0;
        }
}

void Matrix4X4::toPosmat()
{
    for(int i = 0; i < 4; i++)
    {
        if(component[i].component[3] !=0 && component[i].component[3] != 1 )
        {
            for(int j = 0; j < 4; j++)
            {
                component[i].component[j] /=  component[i].component[3];
            }
        }
    }
}

