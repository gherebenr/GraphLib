#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include <cmath>
#include <string>
#include "Color.h"

struct SPoint
{
    float x,y,z;
    SPoint(){x = y = z = 0;}
    SPoint(float a, float b, float c) {x = a; y = b; z = c;}
    
    SPoint operator- (const SPoint& v2)
    {
        SPoint retV;
        retV.x = this->x - v2.x;
        retV.y = this->y - v2.y;
        retV.z = this->z - v2.z;
        return retV;
    }

    SPoint operator+ (const SPoint& v2)
    {
        SPoint retV;
        retV.x = this->x + v2.x;
        retV.y = this->y + v2.y;
        retV.z = this->z + v2.z;
        return retV;
    }

    SPoint operator* (const float& c)
    {
        SPoint retV;
        retV.x = this->x * c;
        retV.y = this->y * c;
        retV.z = this->z * c;
        return retV;
    }

    SPoint operator/ (const float& c)
    {
        SPoint retV;
        retV.x = this->x / c;
        retV.y = this->y / c;
        retV.z = this->z / c;
        return retV;
    }

    SPoint cross(const SPoint& r)
    {
        SPoint ret;
        ret.x = this->y * r.z - this->z * r.y;
        ret.y = this->z * r.x - this->x * r.z;
        ret.z = this->x * r.y - this->y * r.x;
        return ret;
    }

    float dot(const SPoint& r)
    {
        float ret;
        ret = this->x * r.x + this->y * r.y + this->z * r.z;
        return ret;
    }
    
    float length()
    {
        float length = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
        return length;
    }

    float distance(const SPoint& r)
    {
        float x,y,z;
        x = std::abs(this->x - r.x);
        y = std::abs(this->y - r.y);
        z = std::abs(this->z - r.z);
        float dist = sqrt(x * x + y * y + z * z);
        return dist;
    }

    float distanceToSegment(const SPoint& p1, const SPoint& p2)
    {
        SPoint pt2;
        pt2.x = (p1.x + p2.x)/2;
        pt2.y = (p1.y + p2.y)/2;
        pt2.z = (p1.z + p2.z)/2;
        return this->distance(pt2);
    }

    SPoint normalize()
    {
        float length = this->length();
        this->x /= length;
        this->y /= length;
        this->z /= length;
        return *this;
    }
};

struct SVertex : public SPoint
{
    SVertex(){set = false;}
    SPoint normal;
    CColor vColor;
    bool set;
};

struct STriangle
{
    int id;
    SVertex centroid;
    SVertex vertices[3];
    int vertexIndices[3];
    float depthX, depthY, depthZ;
    SPoint triangleNormal;
    SVertex xyProj[3];
    SVertex yzProj[3];
    SVertex zxProj[3];
    bool active;
    CColor defaultColor;
};

struct SShape
{
    int k;
    bool bezier;
    std::vector<SVertex> vertices;
    std::vector<float> knots;
    std::vector<STriangle>triangles;
};

class CShapes
{
public:
    static CShapes* instance();

    void getNextShape();
    void getPreviousShape();

    void addActiveToList();
    void addVertexToActiveShape(SVertex vertex);
    void modifyVertexActiveShape(SVertex vertex, int index);
    void insertVertexActiveShape(SVertex vertex, int index);
    void deleteVertexActiveShape(int index);
    void addTriangleToActiveShape(STriangle triangle);

    void undoVertexAdd();
    void redoVertexAdd();

    std::vector<SShape> getAllShapes() const {return allShapes;}
    SShape getActiveShape() const {return activeShape;}

    void drawAllShapes2D(bool drawOutline, bool useDDA, bool bFill);
    void drawShape2D(SShape _2Dshape, bool drawOutline, bool useDDA, bool bFill, bool active);

    void drawAllShapes3DXY(int xyWindow, SPoint viewPoint, bool wire);
    void drawAllShapes3DYZ(int yzWindow, SPoint viewPoint, bool wire);
    void drawAllShapes3DZX(int zxWindow, SPoint viewPoint, bool wire);

    void translate2D(char direction);
    void rotate2D(float degree);
    void scale2D(float sf);
    void clip2D();

    void translate3D(float x, float y, float z);
    void rotate3D(const SVertex p1, const SVertex p2, float degree);
    void scale3D(float scaleFactorX, float scaleFactorY, float scaleFactorZ);
    
    SVertex intersection(float x1,float y1,float x2,float y2,float x3,float y3,float x4, float y4);
    SVertex findCentroid2D();
    SVertex findCentroid3D();
    static SVertex findCentroid3D(SShape shape);
    static SVertex findCentroid3D(STriangle triangle);

    void calculateTriangleDepths();
    void calculateTriangleDepths(STriangle &triangle, const SShape &shape);
    std::vector <STriangle> sortTrianglesByDepthX();
    std::vector <STriangle> sortTrianglesByDepthY();
    std::vector <STriangle> sortTrianglesByDepthZ();

    static bool depthSortFunctionX(STriangle t1, STriangle t2);
    static bool depthSortFunctionY(STriangle t1, STriangle t2);
    static bool depthSortFunctionZ(STriangle t1, STriangle t2);

    void calculateVertexNormals(SShape &shape);
    void calculateVertexNormals();
    
    void projectTriangleXY(STriangle& triangle);
    void projectTriangleYZ(STriangle& triangle);
    void projectTriangleZX(STriangle& triangle);
    void projectTriangleCAM(STriangle& triangle);

    void drawAllCurves(int resolution);
    void drawCurve(SShape _2Dshape, bool active, int resolution);

    void drawBezier(float t, SShape curve, bool active);
    void drawBezier(SShape curve, bool active, int numPts);

    void drawBSpline(float t, SShape curve, bool active);
    void drawBSpline(SShape curve, bool active, int numPts);

    void setK(int k);
    void addKnot(float knot);
    void modKnot(int index, float knot);

    void toggleBezier(){activeShape.bezier = !activeShape.bezier; activeShape.k = 2;}
    void setBezier(bool bez){activeShape.bezier = bez;}
private:
    static CShapes* thisInstance;

    SShape activeShape;
    std::vector<SShape> allShapes;

    SVertex temp2DVertex;

    CColor notSelected =  CColor(1,1,1);
    CColor notSelectedFill = CColor(0.4,0.4,0.4);
    CColor DDAColor = CColor(0,0,1);
    CColor FillColor = CColor(1,0,0);
    CColor BColor = CColor(0,1,0);
    CColor BSColor = CColor(1,0,0);
    CColor axisColor =  CColor(0.7,0.7,0.7);
};

#endif