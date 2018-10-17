/*******************************************************************************
Copyright (c) 2014, XI'AN JIAOTONG UNIVERSITY
Author Cui Bin
This file is part of the RPmachine Tools suite.
*******************************************************************************/

#ifndef _VEC3D_H
#define _VEC3D_H

//Possible Linux portability issues: min, max

#include <math.h>
#include <float.h>
#include <QDebug>

#ifdef WIN32
#define M_PI 3.14159265358979323846
#endif // WIN32

#define RAD2DEG(_rad) ((_rad)*(180/M_PI))
#define DEG2RAD(_deg) ((_deg)*(M_PI/180))

    enum PrintType
    {
        RP_LINE = 0,
        RP_ARCP,
        RP_ARC
    };

    class Vec2D;
    class Vec4D;
    class CQuat;

    class Vec3D {
    public:
        //Constructors
        Vec3D() {x = 0; y = 0; z = 0;}
        //copy constructor
        Vec3D(const Vec3D& Vec) {*this = Vec;}

        Vec3D(double dx, double dy, double dz) {x = dx; y = dy; z = dz;}
        //overload equals
        inline Vec3D& operator=(const Vec3D& s) {x = s.x; y = s.y; z = s.z; return *this; }


    #ifdef WIN32
        bool IsValid() const {return !_isnan(x) && _finite(x) && !_isnan(y) && _finite(y) && !_isnan(z) && _finite(z);} //is a valid vector? (funky windows _ versions...)
    #else
        bool IsValid() const {return !isnan(x) && finite(x) && !isnan(y) && finite(y) && !isnan(z) && finite(z);} //is a valid vector?
    #endif

        bool IsNear(Vec3D& s, double thresh) {return Dist2(s)<thresh*thresh;}

        //Attributes
        double x, y, z;
        inline double getX()const{return x;}
        inline double getY()const{return y;}
        inline double getZ()const{return z;}
        inline void setX(double XIn) {x = XIn;}
        inline void setY(double YIn) {y = YIn;}
        inline void setZ(double ZIn) {z = ZIn;}


        //these should all be friends....
        //Plus
        friend Vec3D operator+(const Vec3D& v1, const Vec3D& v2) {return Vec3D(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);}
        //Minus
        friend Vec3D operator-(const Vec3D& v1, const Vec3D& v2) {return Vec3D(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);}
        //Multiply
        friend Vec3D operator*(const Vec3D& v, const double f) {return Vec3D(v.x*f, v.y*f, v.z*f);}
        friend Vec3D operator*(const double f, const Vec3D& v) {return v*f;}
        //Divide
        friend Vec3D operator/(const Vec3D& v, const double f) {return Vec3D(v.x/f, v.y/f, v.z/f);}
        //Negative
        friend Vec3D operator-(const Vec3D& v) {return Vec3D(-v.x, -v.y, -v.z);}
        //Is equal
        friend bool operator==(const Vec3D& v1, const Vec3D& v2) {return (v1.x==v2.x && v1.y==v2.y && v1.z==v2.z);}
        //Is not equal
        friend bool operator!=(const Vec3D& v1, const Vec3D& v2) {return !(v1==v2);}
        //interior...
        //add and set
        Vec3D& operator+=(const Vec3D& s) {x += s.x; y += s.y; z += s.z; return *this;}
        //subract and set
        Vec3D& operator-=(const Vec3D& s) {x -= s.x; y -= s.y; z -= s.z; return *this;}
        //multiply and set
        Vec3D& operator*=(const double f) {x *= f; y *= f; z *= f; return *this;}
        //subtract and set
        Vec3D& operator/=(const double f) {x /= f; y /= f; z /= f; return *this;}

        //Vector operations (change this object)
        //normalizes this vector, return module.
        inline double Normalize() {double l = sqrt(x*x+y*y+z*z); if (l > 0) {x /= l;y /= l;z /= l;} return l;}
        //rotates by arbitrary vector arbitrary amount (http://www.cprogramming.com/tutorial/3d/rotation.html (Errors! LH one is actually RH one))
        inline Vec3D Rot(Vec3D u, double a) {double ca = cos(a); double sa = sin(a); double t = 1-ca; return Vec3D((u.x*u.x*t + ca) * x + (u.x*u.y*t - u.z*sa) * y + (u.z*u.x*t + u.y*sa) * z, (u.x*u.y*t + u.z*sa) * x + (u.y*u.y*t+ca) * y + (u.y*u.z*t - u.x*sa) * z, (u.z*u.x*t - u.y*sa) * x + (u.y*u.z*t + u.x*sa) * y + (u.z*u.z*t + ca) * z);}
        inline Vec3D Rot(CQuat& Q); //below CQuat for linking sake...

        //rotates about Z axis "a" radians  anticlockwise.
        void RotZ(double a) {double xt =  x*cos(a) - y*sin(a); double yt = x*sin(a) + y*cos(a); x = xt; y = yt;}
        //rotates about Y axis "a" radians
        void RotY(double a) {double xt =  x*cos(a) + z*sin(a); double zt = -x*sin(a) + z*cos(a); x = xt; z = zt;}
        //rotates about X axis "a" radians
        void RotX(double a) {double yt =  y*cos(a) + z*sin(a); double zt = -y*sin(a) + z*cos(a); y = yt; z = zt;}

        //Vector operations (don't change this object!)
        //Cross product
        inline Vec3D Cross(Vec3D& v) {return Vec3D(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x);}

        friend Vec3D Cross2(Vec3D& v1,Vec3D& v2) {return Vec3D(v1.y*v2.z-v1.z*v2.y,  v1.z*v2.x-v1.x*v2.z, v1.x*v2.y-v1.y*v2.x);}

        inline double Sine(Vec3D& v) {return (x*v.y-y*v.x);}
        inline double Sine(const Vec3D& v) {return (x*v.y-y*v.x);}


        //Dot product
        inline double Dot(Vec3D& v) {return (x * v.x + y * v.y + z * v.z);}
        inline double Dot(const Vec3D &v) {return (x * v.x + y * v.y + z * v.z);}
        //Absolute value
        inline Vec3D Abs() {return Vec3D(fabs(x),fabs(y),fabs(z));}
        //returns normalized vec, change this object.
        inline Vec3D Normalized() {	double l = sqrt(x*x+y*y+z*z); return (l>0?(*this)/l:(*this));}
        //projection into the xy plane
        inline Vec3D ProjXY() {	return Vec3D(x,y,0);}
        //length of the vector
        inline double Length() {return sqrt(x*x+y*y+z*z);}
        //length squared of the vector
        inline double Length2() {return (x*x+y*y+z*z);}
        //min vector of the two
        inline Vec3D Min(const Vec3D& s) {return Vec3D(x<s.x ? x:s.x, y<s.y ? y:s.y, z<s.z ? z:s.z);}
        //max vector of the two
        inline Vec3D Max(const Vec3D& s) {return Vec3D(x>s.x ? x:s.x, y>s.y ? y:s.y, z>s.z ? z:s.z);}
        //minimum element of this vector
        inline double Min() {double Min1 = (x<y ? x:y); return (z<Min1 ? z:Min1);}
         //max element of this vector
        inline double Max() {double Max1 = (x>y ? x:y); return (z>Max1 ? z:Max1);}
        //scales by another vector
        inline Vec3D Scale(const Vec3D& v) {return Vec3D(x*v.x, y*v.y, z*v.z);}
        //scales by inverse of another vector
        inline Vec3D ScaleInv(const Vec3D& v) {return Vec3D(x/v.x, y/v.y, z/v.z);}
         //distance from another vector
        inline double Dist(const Vec3D& v) {return sqrt(Dist2(v));}
         //distance from another vector
        inline double Dist2(const Vec3D& v) {return (v.x-x)*(v.x-x)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z);}
        //returns vector (rotax) and amount (return val) to align this vector with target vector
        inline double AlignWith(Vec3D target, Vec3D& rotax) {Vec3D thisvec = Normalized(); Vec3D targvec = target.Normalized(); Vec3D rotaxis = thisvec.Cross(targvec); if (rotaxis.Length2() == 0) {rotaxis=target.ArbitraryNormal();} rotax = rotaxis.Normalized(); return acos(thisvec.Dot(targvec));}
        //generate arbitrary normal
        inline Vec3D ArbitraryNormal() {Vec3D n = Normalized(); if (fabs(n.x) <= fabs(n.y) && fabs(n.x) <= fabs(n.z)){n.x = 1;} else if (fabs(n.y) <= fabs(n.x) && fabs(n.y) <= fabs(n.z)){n.y = 1;}	else {n.z = 1;}	return Cross(n).Normalized();}
        //vertical
        inline void VerticalXY(Vec3D& a,Vec3D& b){a.x = -y; a.y = x; b.x = y; b.y = -x;}
        inline Vec3D VerticalXYLeft(){this->Normalize();return Vec3D(-y,x,0);}
        inline Vec3D VerticalXYRight(){this->Normalize();return Vec3D(y,-x,0);}
        inline Vec3D inverse(){this->x = -x; this -> y = -y; this->z = -z; return (*this);}

        inline bool IsApproximateEqual(const Vec3D &v, double times)
        {
            double coef = pow(10, times);
            double sx,sy,sz,tx,ty,tz;
            sx = floor(x*coef)/coef;   sy = floor(y*coef)/coef;   sz = floor(z*coef)/coef;
            tx = floor(v.x*coef)/coef; ty = floor(v.y*coef)/coef; tz = floor(v.z*coef)/coef;
            return(sx==tx && sy == ty && sz ==tz);
        }

        friend bool IsClockWise(Vec3D& a,Vec3D& b,Vec3D &c)
        {
            Vec3D v1 = b - a;
            Vec3D v2 = c - b;
            Vec3D vz = v1.Cross(v2);

            if(vz.getZ() < 0)
                return true;

            return false;
        }

        friend Vec3D Mid(Vec3D& v1,Vec3D& v2) {Vec3D v3; v3 = v1-v2; return v3/2;}

        Vec4D toVec4D();
        Vec2D toVec2D();

    };


    //quaternion properties (for my reference)
    //1) To rotate a vector V, form a quaternion with w = 0; To rotate by Quaternion Q, do Q*V*Q.Conjugate() and trim off the w component.
    //2) To do multiple rotations: To Rotate by Q1 THEN Q2, Q2*Q1*V*Q1.Conjugate*Q2.Conjugate(), or make a Qtot = Q2*Q1 and do Qtot*V*Qtot.Conjucate()
    //3) Q1*Q1.Conjugate - Identity
    //4) To do a reverse rotation Q1, just do Q1.conjugate*V*Q1
    //http://www.cprogramming.com/tutorial/3d/quaternions.html

    // public Vec3D //extending Vec3D saves us reimplementing some stuff, I think? this is not a comprehensive quaternion class at this point...
    class CQuat
    {
    public:
        CQuat(void) {Clear();}
        ~CQuat(void){}
        //constructor
        CQuat(double dw, double dx, double dy, double dz) {w=dw; x=dx; y=dy; z=dz;}
        //copy constructor
        CQuat(const CQuat& Quat) {*this = Quat;}
        CQuat(const Vec3D& VecIn) {w = 0; x = VecIn.x; y = VecIn.y; z = VecIn.z;}
        CQuat(double angle, const Vec3D &axis){Clear(); const double a = angle * 0.5f; const double s = sin(a); const double c = cos(a); w = c; x = axis.x * s; y = axis.y * s; z = axis.z * s;}
        //overload equals
        inline CQuat& operator=(const CQuat& s) {w = s.w; x = s.x; y = s.y; z = s.z; return *this; }
        // for (int i=0; i<3; i++){for (int j=0; j<3; j++){M[i][j] = 0;iM[i][j] = 0;}}};
        inline void Clear() {w=1; x=0; y=0; z=0;}
        //shouldnt be necessary... should be able to just set equal...
        inline Vec3D ToVec(void) {return Vec3D(x, y, z);}
        //Plus
        friend CQuat operator+(const CQuat& s1, const CQuat& s2) {return CQuat(s1.w+s2.w, s1.x+s2.x, s1.y+s2.y, s1.z+s2.z);}
         //add and set
        CQuat& operator+=(const CQuat& s) {w += s.w; x += s.x; y += s.y; z += s.z; return *this;}
        //Minus
        friend CQuat operator-(const CQuat& s1, const CQuat& s2) {return CQuat(s1.w-s2.w, s1.x-s2.x, s1.y-s2.y, s1.z-s2.z);}
        //subract and set
        CQuat& operator-=(const CQuat& s) {w -= s.w; x -= s.x; y -= s.y; z -= s.z; return *this;}
        //scalar multiplication
        inline CQuat operator*(double f) {return CQuat(w*f, x*f, y*f, z*f);}
        friend CQuat operator*(double f, CQuat v) {return CQuat(v.w*f, v.x*f, v.y*f, v.z*f);}
        //overload Quaternion multiplication!
        inline CQuat operator*(const CQuat& f) {return CQuat(w*f.w - x*f.x - y*f.y - z*f.z, w*f.x + x*f.w + y*f.z - z*f.y, w*f.y - x*f.z + y*f.w + z*f.x, w*f.z + x*f.y - y*f.x + z*f.w);}
        operator Vec3D() {return Vec3D(x, y, z);}
        //Is equal
        friend bool operator==(const CQuat q1, const CQuat& q2) {return (q1.w==q2.w && q1.x==q2.x && q1.y==q2.y && q1.z==q2.z);}
         //Is not equal
        friend bool operator!=(const CQuat q1, const CQuat& q2) {return (q1.w!=q2.w || q1.x!=q2.x || q1.y!=q2.y || q1.z!=q2.z);}

        //length of the vector
        inline double Length() {return sqrt(Length2());}
         //length squared of the vector
        inline double Length2() {return (w*w+x*x+y*y+z*z);}
        double Normalize(void) {double l = Length(); if (l == 0){w = 1; x = 0; y = 0; z = 0;} else if (l > 0) {w /= l; x /= l; y /= l; z /= l;} return l;}
        CQuat Inverse(void) {double n = w*w + x*x + y*y + z*z; return CQuat(w/n, -x/n, -y/n, -z/n); }
        CQuat Conjugate(void) {return CQuat(w, -x, -y, -z);}

        double w, x, y, z;

        double M [3][3];
        double iM [3][3];
        void CalcMatrix(void) {
            M[0][0] = 1.0f-2.0f*(y*y+z*z);
            M[0][1] = 2.0f*(y*x-z*w);
            M[0][2] = 2.0f*(z*x+y*w);
            M[1][0] = 2.0f*(y*x+z*w);
            M[1][1] = 1.0f-2.0f*(x*x+z*z);
            M[1][2] = 2.0f*(z*y-x*w);
            M[2][0] = 2.0f*(z*x-y*w);
            M[2][1] = 2.0f*(z*y+x*w);
            M[2][2] = 1.0f-2.0f*(x*x+y*y);}

        void CalciMatrix(void) {double determinant = -M[0][2]*M[1][1]*M[2][0] + M[0][1]*M[1][2]*M[2][0] + M[0][2]*M[1][0]*M[2][1] - M[0][0]*M[1][2]*M[2][1] - M[0][1]*M[1][0]*M[2][2] + M[0][0]*M[1][1]*M[2][2]; double k = 1.0f / determinant; iM[0][0] = (M[1][1]*M[2][2] - M[2][1]*M[1][2])*k; iM[0][1] = (M[2][1]*M[0][2] - M[0][1]*M[2][2])*k; iM[0][2] = (M[0][1]*M[1][2] - M[1][1]*M[0][2])*k; iM[1][0] = (M[1][2]*M[2][0] - M[2][2]*M[1][0])*k; iM[1][1] = (M[2][2]*M[0][0] - M[0][2]*M[2][0])*k; iM[1][2] = (M[0][2]*M[1][0] - M[1][2]*M[0][0])*k; iM[2][0] = (M[1][0]*M[2][1] - M[2][0]*M[1][1])*k; iM[2][1] = (M[2][0]*M[0][1] - M[0][0]*M[2][1])*k; iM[2][2] = (M[0][0]*M[1][1] - M[1][0]*M[0][1])*k;}
        void AngleAxis(double &angle, Vec3D &axis) {if (w>1.0f) w = 1.0f; double squareLength = x*x + y*y + z*z; if (squareLength>0.0000000000001f){angle = 2.0f * (double) acos(w); double inverseLength = 1.0f / (double) pow(squareLength, 0.5); axis.x = x * inverseLength; axis.y = y * inverseLength; axis.z = z * inverseLength;} else{angle = 0.0f; axis.x = 1.0f; axis.y = 0.0f; axis.z = 0.0f;}}
    };


    inline Vec3D Vec3D::Rot(CQuat& Q){return (Q*CQuat(*this)*Q.Conjugate()).ToVec();}


    class Line
    {
      protected:
        Vec3D lhead;
        Vec3D ltail;
        Vec3D normal;

       public:
        Line():lhead(Vec3D()),ltail(Vec3D())
        {
        }

        Line(const Vec3D &shead, const Vec3D &stail)
        {
            lhead = shead;
            ltail = stail;
            setNormal();
        }

        void setNormal()
        {
            normal = ltail - lhead;
            normal.Normalize();
        }

        void setHead(Vec3D shead)
        {
           lhead = shead;
        }

        void setEnd(Vec3D stail)
        {
           ltail = stail;
        }

        Vec3D getMiddle() const
        {
            Vec3D middle = ltail - lhead;
            middle /= 2;
            Vec3D point = lhead + middle;
            return point;
        }

        Vec3D getHead() const
        {
            return lhead;
        }

        Vec3D getTail() const
        {
            return ltail;
        }

        Vec3D getVector() const
        {
           return normal;
        }

        double distance2Point(Vec3D point)
        {
            Vec3D lstake = normal.VerticalXYLeft();
            Vec3D tail2Pointvec = ltail - point;
            return fabs(tail2Pointvec.Dot(lstake));
        }

        double Point2line(Vec3D point, Vec3D &intersecpoint, bool &isend)
        {
            double lside,lenline,distance,lenhead,lentail,vcos, vsin;
            Vec3D lstake;

            Vec3D tail2Pointvec = ltail - point;
            Vec3D vhead = point - lhead;
            Vec3D vtail = point - ltail;

            lenhead = vhead.Length();
            lentail = vtail.Length();

            //point not intersection line.
            if(vhead.Dot(vtail) > 0)
            {
                if(lenhead < lentail)
                {
                    intersecpoint = lhead;
                    distance = lenhead;
                }
                else
                {
                    intersecpoint = ltail;
                    distance = lentail;
                }
                isend = true;
                return distance;
            }

            lstake = normal.VerticalXYLeft();
            distance = fabs(tail2Pointvec.Dot(lstake));

            lenline = tail2Pointvec.Length();
            tail2Pointvec.Normalize();

            vcos = tail2Pointvec.Dot(lstake);
            vsin = sqrt(1 - vcos*vcos);
            lside = lenline * vsin;

            intersecpoint = ltail - lside*normal;
            isend = false;
            return distance;
        }

        Vec3D intersect(Line &other)
        {
            double parallel = normal.Dot(other.normal)*normal.Dot(other.normal);

            if(parallel >=1)
            {
                return other.ltail;
            }

            double dist = distance2Point(other.getTail());
            double length = dist/sqrt(1 - parallel);
            return other.getTail() - (other.normal * length);
        }

        void translate(const Vec3D &vec, double length)
        {
            lhead = lhead + vec *length;
            ltail = ltail + vec *length;
            setNormal();
        }

        Line& operator=(const Line &sline)
        {
            lhead = sline.getHead();
            ltail = sline.getTail();
            normal = sline.normal;
            return *this;
        }

        bool isPointOnLine(const Vec3D &spoint)
        {
            double cross;

            Vec3D tail2point = spoint - ltail;
            tail2point.Normalize();
            setNormal();

            cross = tail2point.getX()*normal.getY() - tail2point.getY()*normal.getX();
            //delete some deviate.
            if(fabs(cross) > 0.001)
            {
                return false;
            }

            Vec3D head2point = spoint - lhead;
            head2point.Normalize();

            if(head2point.Dot(tail2point) < 0)
            {
                return true;
            }
                return false;
        }

        double getLength() const
        {
            Vec3D  vec = ltail - lhead;
            return  vec.Length();
        }

        Line getCursor(double slength)
        {
            Vec3D tail = getMiddle();
            Vec3D vec = normal.VerticalXYLeft();
            Vec3D head = tail - vec*slength;
            Line result = Line(head,tail);
            return result;
        }

        void getVertials(Vec3D &left, Vec3D &right)
       {
           left = normal.VerticalXYLeft();
           right = normal.VerticalXYRight();
       }

       void getCorners(Vec3D &leftstart,Vec3D &rightstart,Vec3D &leftend,Vec3D &rightend,double radius)
       {
             Vec3D left = normal.VerticalXYLeft();
             Vec3D right = normal.VerticalXYRight();

             leftstart = left  * radius + lhead;
             rightstart= right * radius + lhead;
             leftend = left  * radius + ltail;
             rightend= right * radius + ltail;
       }

        double Dot(const Line &other)
        {
           return this->getVector().Dot(other.getVector());
        }

        double Cross(const Line &other)
        {
           return this->getVector().Sine(other.getVector());
        }

        Vec3D Cross3D(const Line &other)
        {
           Vec3D otherNormal = other.getVector();
           Vec3D tnormal = this->getVector().Vec3D::Cross(otherNormal);
           tnormal.Normalize();
           return tnormal;
        }

        void reverseLine()
        {
           Vec3D point;
           point = ltail;
           ltail = lhead;
           lhead = point;
           setNormal();
        }
    };

    //Plane
    class Plane
    {
     private:
        Vec3D normal;
        double offset;
        Vec3D planep;

     public:
        Plane()
        {
        }

        Plane(Vec3D &sNormal, Vec3D &sp)
        {
            normal = sNormal.Normalized();
            offset = normal.Dot(sp);
            planep = sp;
        }

        //!Copy constructor
        Plane(const Plane& source):
             normal(source.normal),offset(source.offset),planep(source.planep)
        {
        }

        Plane &operator =(const Plane &other)
        {
            normal = other.getNormal();
            offset = other.getOffset();
            return *this;
        }
         //!Returns plane's normal vector
        Vec3D getNormal()
        {
            return normal;
        }

        Vec3D getNormal() const
        {
            return normal;
        }

        //!Sets a new normal vector,with normalize.
        Plane& setNormal(const Vec3D& newNormal)
        {
            normal = newNormal;
            normal.Normalize();
            return *this;
        }

        //!Returns plane's offset from origin
        double getOffset()
        {
            return offset;
        }

        double getOffset() const
        {
            return offset;
        }

        //! Sets a new offset from origin
        Plane& setOffset(double newOffset)
        {
            offset=newOffset;
            return *this;
        }

        //!Sets the plane to contain given point
        Plane& setPoint(const Vec3D& p)
        {
            planep = p;
            offset=normal.Dot(planep);
            return *this;
        }

         //!Sets the plane to contain given normal and point
        Plane& reset(const Vec3D& newNormal,const Vec3D& p)
        {
           this->setNormal(newNormal);
           this->setPoint(p);
           return *this;
        }


        Vec3D getPoint()
        {
            return planep;
        }

        //!< Normalizes the plane's normal vector
        Plane& normalize()
        {
            double normalLen=normal.Normalize();
            offset/=normalLen;
            return *this;
        }

        //! Returns non-normalized signed distance between plane and given point
        double calcDistance(Vec3D& p)
        {
            return normal.Dot(p)-offset;
        }

         //!Returns true if given point is on the plane
        bool isOnPlane(Vec3D& p)
        {
            return normal.Dot(p) == offset;
        }

        //! Returns true if given point is on the plane or in the halfspace behind the plane
        bool isContains(Vec3D& p)
        {
        return normal.Dot(p) <= offset;
        }

        bool intersect(Vec3D &start, Vec3D &end, Vec3D &crossp)
        {
            double product,distance;

            Vec3D vecLine = (end-start);
            product = normal.Dot(vecLine); //project on normal

            // paralle with plane, we find points on the link line of this line.
            if(product == 0)
                 return false;

            distance = calcDistance(end);
            if(distance < 0 && product <0)
            {
              if(distance < product)
                  return false;
            }
            else if(distance > 0 && product > 0)
            {
                if(distance > product)
                    return false;
            }
            else
            {
                return false;
            }

            vecLine.Normalize();
            product = normal.Dot(vecLine);
            distance /= product;

            crossp = end - vecLine*distance;
            return true;
        }

    };

    class RPDisk
    {

    private:

        Vec3D center;
        double radius;
        int status;

    public:

        RPDisk():center(Vec3D()),radius(1),status(0)
        {}

        RPDisk(const Vec3D &scenter,double sradius):status(0)
        {
            center = scenter;
            radius = sradius;
        }

        RPDisk(const Vec3D &scenter,double sradius, int sstatus)
        {
            center = scenter;
            radius = sradius;
            status = sstatus;
        }

        //! from startPos to endPos, if radius = -1 major arc.
        RPDisk(const Vec3D &startPos,const Vec3D &endPos,double sradius):status(0)
        {
            if(sradius == -1)
               radius = -sradius;
            else
               radius =  sradius;


            Vec3D vec = endPos - startPos;
            double len = vec.Length()/2.0;
            vec.Normalize();

            if(len == radius)
            {
                center = startPos + vec*radius;
                return;
            }


            double cosine = len/radius;
            double theta = acos(cosine);

            if(sradius < 0)
            {
                vec.RotZ(theta);
            }
            else
            {
                vec.RotZ(-theta);
            }
            center = startPos + vec*radius;
        }

        RPDisk & operator =(const RPDisk &other)
        {
            center = other.center;
            radius = other.radius;
            status = other.status;
            return *this;
        }

        void setStatus(int sst)
        {
            status = sst;
        }

        int getStatus() const
        {
            return status;
        }

        void setRadius(double sradius)
        {
            radius = sradius;
        }

        double getRadius() const
        {
            return radius;
        }

        void setCenter(const Vec3D &scenter)
        {
            center = scenter;
        }

        Vec3D getCenter() const
        {
            return center;
        }


        //! line1 and lin2 must head to end on same direction.
        Vec3D getCenter(const Vec3D shead, const Vec3D smid,const Vec3D send, Vec3D &intersec1, Vec3D &intersec2)
        {
         Line line[2];
         Vec3D lvector[2],intersecPoint,v0left,v1left,vrot;
         double vcos,theta,angle,longside,wingside,clockwise;
         line[0] = Line(shead,smid);
         line[1] = Line(smid,send);
         //get line intersection point.
         intersecPoint = smid;
         //normalize lines.
         for(int i = 0; i < 2; i ++)
         {
             line[i].setNormal();
             lvector[i] = line[i].getVector();
         }


         //calculate angle and length.
         v0left = lvector[0].VerticalXYLeft();
         v1left = lvector[1].VerticalXYLeft();
          //rotting direction, centerly >0, because vector dot it's leftvector.
         //clockwise = lvector[1].getX()*vleft.getY() - lvector[1].getY()*vleft.getX();
         clockwise = v0left.getX()*v1left.getY() - v0left.getY()*v1left.getX();

         //two line parallel.
         if(clockwise == 0)
         {
           intersec1 = shead;
           intersec2 = shead;
           return shead;
         }
         //actual intersection angle.
         vcos = -lvector[0].Dot(lvector[1]);
         angle = (acos(vcos))/2;
         //orgin intersection angle.
         vcos =  -vcos;
         theta = (acos(vcos));
         //sides.
         longside = radius/(sin(angle));
         wingside = longside*cos(angle);
         //rotate second line to find the direction to center.
         vrot = lvector[1];
         if(clockwise > 0)
             vrot.RotZ(angle);
         else
             vrot.RotZ(theta + angle);

         //calculate results.
         center = intersecPoint + vrot*longside;
         //search intresection point.
         if(clockwise > 0)
         {
            intersec1 = intersecPoint - lvector[0]*wingside;
            intersec2 = intersecPoint + lvector[1]*wingside;
         }
         else
         {
            intersec1 = intersecPoint + lvector[0]*wingside;
            intersec2 = intersecPoint - lvector[1]*wingside;
         }
         return center;
        }


        //! line1 and lin2 must head to end on same direction.
        Vec3D getCenter(const Vec3D shead, const Vec3D smid,const Vec3D send, bool &concave, Vec3D &intersec1, Vec3D &intersec2)
        {
         Line line[2];
         Vec3D lvector[2],intersecPoint,v0left,v1left,vrot;
         double vcos,theta,angle,longside,wingside,clockwise;
         line[0] = Line(shead,smid);
         line[1] = Line(smid,send);
         //get line intersection point.
         intersecPoint = smid;
         //normalize lines.
         for(int i = 0; i < 2; i ++)
         {
             line[i].setNormal();
             lvector[i] = line[i].getVector();
         }
         //calculate angle and length.
         v0left = lvector[0].VerticalXYLeft();
         v1left = lvector[1].VerticalXYLeft();
          //rotting direction, centerly >0, because vector dot it's leftvector.
         //clockwise = lvector[1].getX()*vleft.getY() - lvector[1].getY()*vleft.getX();
         clockwise = v0left.getX()*v1left.getY() - v0left.getY()*v1left.getX();

         //two line parallel.
         if(clockwise == 0)
         {
           concave = true;
           intersec1 = shead;
           intersec2 = shead;
           return shead;
         }
         //actual intersection angle.
         vcos = -lvector[0].Dot(lvector[1]);
         angle = (acos(vcos))/2;
         //orgin intersection angle.
         vcos =  -vcos;
         theta = (acos(vcos));
         //sides.
         longside = radius/(sin(angle));
         wingside = longside*cos(angle);
         //rotate second line to find the direction to center.
         vrot = lvector[1];

         if(clockwise > 0)
         {
              vrot.RotZ(angle);
              center = intersecPoint + vrot*longside;
              concave = false;
         }
         else
         {
              vrot.RotZ(theta + angle);
              center = intersecPoint + vrot*radius;
              concave = true;
         }
         //calculate results.

         //search intresection point.
         if(clockwise > 0)
         {
            intersec1 = intersecPoint - lvector[0]*wingside;
            intersec2 = intersecPoint + lvector[1]*wingside;
         }
         else
         {
            intersec1 = intersecPoint;
            intersec2 = intersecPoint;
         }

         return center;
        }

        Vec3D getInternalCenter(const Vec3D shead, const Vec3D smid,const Vec3D send,Vec3D &vrot)
        {
            double sinangle = 0;
            Line line[2];
            Vec3D lvector[2],intersecPoint,v0left;
            double vcos,angle,longside,clockwise;
            line[0] = Line(shead,smid);
            line[1] = Line(smid,send);
            //get line intersection point.
            intersecPoint = smid;
            //normalize lines.
            for(int i = 0; i < 2; i ++)
            {
              line[i].setNormal();
              lvector[i] = line[i].getVector();
            }

            v0left = lvector[0].VerticalXYLeft();

            clockwise = lvector[0].getX()*lvector[1].getY() - lvector[0].getY()*lvector[1].getX();

            //two line are  parallel.
            if(clockwise == 0)
            {
                 if(shead == send)  //two line overlap.
                {
                    qDebug()<<"two line are overlapped"<<endl;
                }
                return smid + v0left * radius;
            }
            //actual intersection angle. pi - x
            vcos = -lvector[0].Dot(lvector[1]);
            if(vcos > 1)
                vcos = 1;

            if(vcos < -1)
                vcos =-1;

            angle = (acos(vcos))/2;

            //sides.
            sinangle = (sin(angle));

            if(sinangle == 0 )
                longside = radius;
            else
                longside = radius/sinangle;

            //rotate second line to find the direction to center.
            vrot = lvector[1];

            if(clockwise > 0)
            {
                vrot.RotZ(angle);
                center = intersecPoint + vrot*longside;
            }
            else
            {
                vrot.RotZ(-angle);
                center = intersecPoint + vrot*longside;
            }

#ifdef WIN32
            if(_isnan(center.getX())|| !_finite(center.getX())||
               _isnan(center.getY())|| !_finite(center.getY()))
#else
            if(isinf(center.getX())|| isnan(center.getX())||
               isinf(center.getY())|| isnan(center.getY()))
#endif
            {
                qDebug()<<"get ball center error"<<endl;
            }
            return center;
        }

        //! is disk intersected with the line.
        bool isIntersect(const Line &sline)
        {
            double distcenter;
            Vec3D intersecpoint;
            bool isend = false;
            Line line = sline;

            distcenter = line.Point2line(center, intersecpoint, isend);

            //the line is not contact disk.
            if(distcenter >= radius)
            {
                return false;
            }

            return true;
        }
    };

    class RPprintItem
    {
    public:
        RPprintItem():send(Vec3D()),radius(0),radian(0),direction(0),center(Vec3D()){}

        //! arcp
        RPprintItem(Vec3D spos,double sradius,short sdirect):rptype(RP_ARCP)
        {

            send = spos;
            radius = sradius;
            direction = sdirect;

            if(direction != -1 && direction != 1  )
                qDebug()<<"arc must has a direction!"<<endl;
        }
        //!arcp
        RPprintItem(Vec3D spos,double sradius,short sdirect,Vec3D scenter):radian(0),rptype(RP_ARCP)
        {
            send = spos;
            radius = sradius;
            direction = sdirect;
            center = scenter;

            if(direction != -1 && direction != 1  )
                qDebug()<<"arc must has a direction!"<<endl;
        }

        //!arc
        RPprintItem(double sradius,double sradian,Vec3D scenter,Vec3D spos):radian(0), direction(0),rptype(RP_ARC)
        {
            radius = sradius;
            radian = sradian;
            center = scenter;
            send = spos;
        }

        //! line.
        RPprintItem(Vec3D spos):radius(0),radian(0),direction(0),center(Vec3D()),rptype(RP_LINE)
        {
            send = spos;
        }

        ~RPprintItem(){}

        void setZ(double sz)
        {
            send.setZ(sz);
            center.setZ(sz);
        }

        Vec3D getEndPos() const
        {
            return send;
        }

        double getRadius() const
        {
            return radius;
        }

        int getType() const
        {
            return rptype;
        }

        Vec3D getCenter() const
        {
            return center;
        }

        double getRadian() const
        {
            return radian;
        }

        short getDirection() const
        {
            return direction;
        }

        RPprintItem & operator= (const RPprintItem &other)
        {
            this->send = other.getEndPos();
            this->radius = other.getRadius();
            this->radian = other.getRadian();
            this->direction = other.getDirection();
            this->center = other.getCenter();
            this->rptype = other.getType();
            return *this;
        }

    private:
        Vec3D  send;
        double radius;
        double radian;
        short  direction;
        Vec3D  center;
        int rptype;
    };


#endif //_VEC3D_H
