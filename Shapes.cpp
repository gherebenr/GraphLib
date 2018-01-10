#include "Shapes.h"
#include "GraphLib.h"
#include "Color.h"
#include "PixelBuffer.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#define M_PI   3.14159265358979323846 

CShapes* CShapes::thisInstance = new CShapes;

CShapes* CShapes::instance()
{
    if(thisInstance == nullptr)
    {
        thisInstance = new CShapes;
    }
    return thisInstance;
}

void CShapes::drawAllShapes2D(bool drawOutline, bool useDDA, bool bFill)
{
     for(SShape  tShape : allShapes)
    {
        drawShape2D(tShape, drawOutline, useDDA, bFill, false);
    }
    drawShape2D(activeShape, drawOutline, useDDA, bFill, true);
}

void CShapes::drawShape2D(SShape _2Dshape, bool drawOutline, bool useDDA, bool bFill, bool active)
{
    float zoom = CPixelBuffer::instance(0)->getZoom();
    CColor fillColor = notSelectedFill;
    CColor DDAc = notSelected;
    CColor Bc = notSelected;
    CColor pt = notSelected;
    if(active)
    {
        fillColor = FillColor;
        DDAc = DDAColor;
        Bc = BColor;
        pt = Bc;
        if(useDDA){pt = DDAc;}
    }
    if(_2Dshape.vertices.size() == 1)
    {
        CGraphLib::drawPoint2D(_2Dshape.vertices[0].x * zoom, _2Dshape.vertices[0].y * zoom, pt, 0);
    } 
    else if(_2Dshape.vertices.size() == 2 )
    {
        for(int i = 0; i < _2Dshape.vertices.size() - 1; i++)
        {
            if(useDDA){CGraphLib::drawLineDDA2D(_2Dshape.vertices[i], _2Dshape.vertices[i+1], DDAc);}
            else{CGraphLib::drawLineB2D(_2Dshape.vertices[i], _2Dshape.vertices[i+1], Bc, 0);}
        }
    }
    else if(_2Dshape.vertices.size() > 2)
    {
        if(bFill)
        {
            CGraphLib::fillShape2D(fillColor, _2Dshape,0);
        }
        if(drawOutline || !bFill)
        {
            for(int i = 0; i < _2Dshape.vertices.size() - 1; i++)
            {
                if(useDDA){CGraphLib::drawLineDDA2D(_2Dshape.vertices[i], _2Dshape.vertices[i+1], DDAc);}
                else{CGraphLib::drawLineB2D(_2Dshape.vertices[i], _2Dshape.vertices[i+1], Bc, 0);}
            }
            if(useDDA){CGraphLib::drawLineDDA2D(_2Dshape.vertices[_2Dshape.vertices.size() - 1], _2Dshape.vertices[0], DDAc);}
                else{CGraphLib::drawLineB2D(_2Dshape.vertices[_2Dshape.vertices.size() - 1], _2Dshape.vertices[0], Bc, 0);}
        }
    }
}

void CShapes::drawAllCurves(int resolution)
{
    for(SShape  tShape : allShapes)
    {
        drawCurve(tShape, false, resolution);
    }
    drawCurve(activeShape, true, resolution);
}

void CShapes::drawCurve(SShape _2Dshape,bool active, int resolution)
{
    float zoom = CPixelBuffer::instance(0)->getZoom();
    bool bezier = _2Dshape.bezier;
    CColor fillColor = notSelectedFill;
    CColor DDAc = notSelected;
    CColor Bc = notSelected;
    CColor pt = notSelected;
    if(active)
    {
        fillColor = FillColor;
        DDAc = DDAColor;
        Bc = BColor;
        pt = Bc;
    }
    if(_2Dshape.vertices.size() > 0)
    {
        for(int i = 0; i < _2Dshape.vertices.size(); i++)
        {
            CGraphLib::drawPoint2D(_2Dshape.vertices[i].x*zoom,_2Dshape.vertices[i].y*zoom, Bc, 0);
            CGraphLib::drawPoint2D(_2Dshape.vertices[i].x*zoom+1,_2Dshape.vertices[i].y*zoom, Bc, 0);
            CGraphLib::drawPoint2D(_2Dshape.vertices[i].x*zoom-1,_2Dshape.vertices[i].y*zoom, Bc, 0);
            CGraphLib::drawPoint2D(_2Dshape.vertices[i].x*zoom,_2Dshape.vertices[i].y*zoom+1, Bc, 0);
            CGraphLib::drawPoint2D(_2Dshape.vertices[i].x*zoom,_2Dshape.vertices[i].y*zoom-1, Bc, 0);
        }
        if(bezier)
        {
            drawBezier(_2Dshape, active, resolution);
        }
        else
        {
            drawBSpline(_2Dshape, active, resolution);
        }
    }
}

void CShapes::drawBezier(float t, SShape curve, bool active)
{
    CColor curveColor = notSelected;
    if(active)
    {
        curveColor = BColor;
    }
    float zoom = CPixelBuffer::instance(0)->getZoom();
    if (curve.vertices.empty()) return;
    int count = curve.vertices.size() - 1;
    std::vector<SPoint> prev, current(curve.vertices.begin(), curve.vertices.end());
    while(count--) {
        prev.assign(current.begin(), current.end());
        current.clear();
        for (int i = 0; i < prev.size() - 1; i++)
        {
            SPoint add = (prev[i] * t) + (prev[i+1] * (1-t));
            current.push_back(add);
        }
    }
    CGraphLib::drawPoint2D(current.front().x*zoom, current.front().y*zoom, curveColor, 0);
}

void CShapes::drawBezier(SShape curve, bool active, int numPts)
{
    float resolution = 1.0f / (numPts - 1);
    for (float t = 0; t <= 1; t += resolution)
        drawBezier(t, curve, active);
}

void CShapes::drawBSpline(float u, SShape curve, bool active)
{
    CColor curveColor = notSelected;
    if(active)
    {
        curveColor = BSColor;
    }
    float zoom = CPixelBuffer::instance(0)->getZoom();
    if (curve.vertices.empty()) return;
    int I;
    bool found = false;
    for (I = curve.k - 1; I < curve.vertices.size(); I++)
        if (curve.knots[I] <= u && u < curve.knots[I + 1]) 
        {
            found = true;
            break;
        }
    if(!found){return;}
    std::vector<SPoint> prev;
    int beg = I - curve.k + 1;
    int end = beg + curve.k;
    std::vector<SPoint> current(curve.vertices.begin() + beg,curve.vertices.begin() + end);
    for (int r = 0; r < curve.k - 1; r++)
    {
        prev.assign(current.begin(), current.end());
        current.clear();
        for (int i = 0; i < prev.size() - 1; i++)
        {
            int leftIndex = I - curve.k + i + r + 2;
            int rightIndex = I + 1 + i;
            float left = curve.knots[leftIndex];
            float right = curve.knots[rightIndex];
            current.push_back((prev[i] * (right - u) + prev[i+1]*(u - left)) / (right - left));
        }
    }
    CGraphLib::drawPoint2D(current.front().x*zoom, current.front().y*zoom, curveColor, 0);
}

void CShapes::drawBSpline(SShape curve, bool active, int numPts)
{
    if (curve.knots.size()) {
        float delta = (curve.knots[curve.vertices.size()] - curve.knots[curve.k - 1]) / (numPts - 1);
        for (float u = curve.knots[curve.k - 1]; u < curve.knots[curve.vertices.size()]; u += delta)
            drawBSpline(u, curve, active);
    }
}

void CShapes::setK(int k)
{
    if(k > activeShape.vertices.size()){k = activeShape.vertices.size();}
    if(k < 2){k = 2;}
    activeShape.k = k;
}

void CShapes::addKnot(float knot)
{
    if(activeShape.knots.empty())
    {
        activeShape.knots.push_back(knot);
        for(int i = 0; i < activeShape.k + activeShape.vertices.size() - 1; i++)
        {
            knot++;
            activeShape.knots.push_back(knot);
        }
        return;
    }
    while(activeShape.knots.size() < activeShape.vertices.size() + activeShape.k)
    {
        if(activeShape.knots[activeShape.knots.size()-1] > knot)
        {
            activeShape.knots.push_back(activeShape.knots[activeShape.knots.size()-1] + 1);
        }
        else
        {
            activeShape.knots.push_back(knot);
        }
    }
}

void CShapes::modKnot(int index, float knot)
{
    activeShape.knots[index] = knot;
}


void CShapes::drawAllShapes3DXY(int xyWindow, bool wire, SPoint viewPoint)
{
    for(SShape& shape3D : allShapes)
    {
        drawShape3DXY(shape3D, false, wire, xyWindow);
    }
    drawShape3DXY(activeShape, true, wire, xyWindow);
    if(!wire)
    {
        std::vector<STriangle> sortedZDepthTriangles = sortTrianglesByDepthZ();
        CGraphLib::calculateLightAtVertices(sortedZDepthTriangles, viewPoint);
        if(sortedZDepthTriangles.size() > 0)
        {
            for(auto &triangle : sortedZDepthTriangles)
            {
                projectTriangle(triangle);
                CGraphLib::drawTriangle(triangle, xyWindow, "xy");
            }
        }
        CGraphLib::drawLightSource("xy", xyWindow);
    }
}

void CShapes::drawAllShapes3DYZ(int yzWindow, bool wire, SPoint viewPoint)
{
    for(SShape& shape3D : allShapes)
    {
        drawShape3DYZ(shape3D, false, wire, yzWindow);
    }
    drawShape3DYZ(activeShape, true, wire, yzWindow);
     if(!wire)
    {
        std::vector<STriangle> sortedXDepthTriangles = sortTrianglesByDepthX();
        CGraphLib::calculateLightAtVertices(sortedXDepthTriangles, viewPoint);
        if(sortedXDepthTriangles.size() > 0)
        {
            for(auto &triangle : sortedXDepthTriangles)
            {
                projectTriangle(triangle);
                CGraphLib::drawTriangle(triangle, yzWindow, "yz");
            }
        }
        CGraphLib::drawLightSource("yz", yzWindow);
    }
}

void CShapes::drawAllShapes3DZX(int zxWindow, bool wire, SPoint viewPoint)
{
    for(SShape& shape3D : allShapes)
    {
        drawShape3DZX(shape3D, false, wire, zxWindow);
    }
    drawShape3DZX(activeShape, true, wire, zxWindow);
     if(!wire)
    {
        std::vector<STriangle> sortedYDepthTriangles = sortTrianglesByDepthY();
        CGraphLib::calculateLightAtVertices(sortedYDepthTriangles, viewPoint);
        if(sortedYDepthTriangles.size() > 0)
        {
            for(auto &triangle : sortedYDepthTriangles)
            {
                projectTriangle(triangle);
                CGraphLib::drawTriangle(triangle, zxWindow, "zx");
            }
        }
        CGraphLib::drawLightSource("zx", zxWindow);
    }
}

void CShapes::drawShape3DXY(SShape& shape, bool active, bool wire, int xyWindow)
{
    float maxCoord = CPixelBuffer::instance(xyWindow)->getMaxCoord();
    float minCoord = CPixelBuffer::instance(xyWindow)->getMinCoord();
    SVertex startV;
    SVertex endV;
    startV.x = (-minCoord);
    startV.y = (minCoord);
    endV.x = (-minCoord);
    endV.y = (maxCoord-minCoord);
    CGraphLib::drawLineB2D(startV, endV, axisColor, xyWindow);
    startV.x = (minCoord);
    startV.y = (-minCoord);
    endV.x = (maxCoord-minCoord);
    endV.y = (-minCoord);
    CGraphLib::drawLineB2D(startV, endV, axisColor, xyWindow);
    CColor Bc = notSelected;
    if(active)
    {
        Bc = BColor;
    }
    if(shape.vertices.size() > 3 && wire)
    {
        for(SEdge edge : shape.edges)
        {
            startV.x = ((shape.vertices[edge.p1-1].x-minCoord));
            startV.y = ((shape.vertices[edge.p1-1].y-minCoord));
            endV.x = ((shape.vertices[edge.p2-1].x-minCoord));
            endV.y = ((shape.vertices[edge.p2-1].y-minCoord));
            CGraphLib::drawLineB2D(startV, endV, Bc, xyWindow);
        }
    }
    if(!wire)
    {
        if(active)
        {
            for(STriangle &triangle : shape.triangles)
            {
                triangle.defaultColor = CColor(0,1,0);
            }
        }
        else
        {
            for(STriangle &triangle : shape.triangles)
            {
                triangle.defaultColor = CColor(0.6,0.5,0.4);
            }
        }
    }
}

void CShapes::drawShape3DYZ(SShape& shape, bool active, bool wire, int yzWindow)
{
    float maxCoord = CPixelBuffer::instance(yzWindow)->getMaxCoord();
    float minCoord = CPixelBuffer::instance(yzWindow)->getMinCoord();
    SVertex startV;
    SVertex endV;
    startV.x = (-minCoord);
    startV.y = (minCoord);
    endV.x = (-minCoord);
    endV.y = (maxCoord-minCoord);
    CGraphLib::drawLineB2D(startV, endV, axisColor, yzWindow);
    startV.x = (minCoord);
    startV.y = (-minCoord);
    endV.x = (maxCoord-minCoord);
    endV.y = (-minCoord);
    CGraphLib::drawLineB2D(startV, endV, axisColor, yzWindow);
    CColor Bc = notSelected;
    if(active)
    {
        Bc = BColor;
    }
    if(shape.vertices.size() > 3 && wire)
    {
        for(SEdge edge : shape.edges)
        {
            startV.x = ((shape.vertices[edge.p1-1].y-minCoord));
            startV.y = ((shape.vertices[edge.p1-1].z-minCoord));
            endV.x = ((shape.vertices[edge.p2-1].y-minCoord));
            endV.y = ((shape.vertices[edge.p2-1].z-minCoord));
            CGraphLib::drawLineB2D(startV, endV, Bc, yzWindow);
        }
    }
}

void CShapes::drawShape3DZX(SShape& shape, bool active, bool wire, int zxWindow)
{
    float maxCoord = CPixelBuffer::instance(zxWindow)->getMaxCoord();
    float minCoord = CPixelBuffer::instance(zxWindow)->getMinCoord();
    SVertex startV;
    SVertex endV;
    startV.x = (-minCoord);
    startV.y = (minCoord);
    endV.x = (-minCoord);
    endV.y = (maxCoord-minCoord);
    CGraphLib::drawLineB2D(startV, endV, axisColor, zxWindow);
    startV.x = (minCoord);
    startV.y = (-minCoord);
    endV.x = (maxCoord-minCoord);
    endV.y = (-minCoord);
    CGraphLib::drawLineB2D(startV, endV, axisColor, zxWindow);
    CColor Bc = notSelected;
    if(active)
    {
        Bc = BColor;
    }
    if(shape.vertices.size() > 3 && wire)
    {
        for(SEdge edge : shape.edges)
        {
            startV.x = ((shape.vertices[edge.p1-1].z-minCoord));
            startV.y = ((shape.vertices[edge.p1-1].x-minCoord));
            endV.x = ((shape.vertices[edge.p2-1].z-minCoord));
            endV.y = ((shape.vertices[edge.p2-1].x-minCoord));
            CGraphLib::drawLineB2D(startV, endV, Bc, zxWindow);
        }
    }
}

void CShapes::getPreviousShape()
{
    if(allShapes.size() > 0)
    {
        if(activeShape.vertices.size() > 0)
        {
            allShapes.insert(allShapes.begin(), activeShape);
        }
        activeShape.vertices.clear();
        activeShape.edges.clear();
        activeShape.triangles.clear();
        activeShape.k = 0;
        activeShape.knots.clear();
        temp2DVertex.set = false;
        activeShape = allShapes[allShapes.size()-1];
        allShapes.erase(allShapes.end());
    }
}

void CShapes::getNextShape()
{
    if(allShapes.size() > 0)
    {
        if(activeShape.vertices.size() > 0)
        {
            allShapes.push_back(activeShape);
        }
        activeShape.vertices.clear();
        activeShape.edges.clear();
        activeShape.triangles.clear();
        activeShape.k = 0;
        activeShape.knots.clear();
        temp2DVertex.set = false;
        activeShape = allShapes[0];
        allShapes.erase(allShapes.begin());
    }
}

void CShapes::addActiveToList()
{
    if(activeShape.vertices.size() > 0)
    {
        allShapes.push_back(activeShape);
    }
    activeShape.vertices.clear();
    activeShape.edges.clear();
    activeShape.triangles.clear();
    activeShape.k = 0;
    activeShape.knots.clear();
}

void CShapes::addVertexToActiveShape(SVertex vertex)
{
    activeShape.vertices.push_back(vertex);
}

void CShapes::modifyVertexActiveShape(SVertex vertex, int index)
{
    if(index < activeShape.vertices.size())
    {
        activeShape.vertices[index] = vertex;
    }
}

void CShapes::insertVertexActiveShape(SVertex vertex, int index)
{
    if(index < activeShape.vertices.size())
    {
        int i = (index + 1) % activeShape.vertices.size();
        activeShape.vertices.insert(activeShape.vertices.begin()+i, vertex);
    }
}

void CShapes::deleteVertexActiveShape(int index)
{
    if(index < activeShape.vertices.size())
    {
        activeShape.vertices.erase(activeShape.vertices.begin()+index);
    }
}

void CShapes::undoVertexAdd()
{
    if(activeShape.vertices.size() > 0)
    {
        temp2DVertex = activeShape.vertices[activeShape.vertices.size()-1];
        temp2DVertex.set = true;
        activeShape.vertices.pop_back();
    }
}

void CShapes::redoVertexAdd()
{
    if(temp2DVertex.set)
    {
        activeShape.vertices.push_back(temp2DVertex);
         temp2DVertex.set = false;
    }
}

void CShapes::addEdgeToActiveShape(SEdge edge)
{
    activeShape.edges.push_back(edge);
}

void CShapes::addTriangleToActiveShape(STriangle triangle)
{
    activeShape.triangles.push_back(triangle);
}

void CShapes::translate2D(char direction)
{
    for(int i = 0; i < activeShape.vertices.size(); i++)
    {
        if(direction == 'u'){activeShape.vertices[i].y += 0.01;}
        if(direction == 'd'){activeShape.vertices[i].y -= 0.01;}
        if(direction == 'r'){activeShape.vertices[i].x += 0.01;}
        if(direction == 'l'){activeShape.vertices[i].x -= 0.01;}
    }
}

void CShapes::rotate2D( float degree)
{
    SVertex centroid = findCentroid2D();
    if(activeShape.vertices.size() > 3)
    {
        centroid = findCentroid3D();
    }
    float cx = centroid.x;
    float cy = centroid.y;
    float x = 0, y = 0;
    for(int i = 0; i < activeShape.vertices.size(); i++)
    {
        x = activeShape.vertices[i].x;
        y = activeShape.vertices[i].y;
        activeShape.vertices[i].x = (x - cx) * cos(degree * M_PI/180) - (y - cy) * sin(degree * M_PI/180) + cx;
        activeShape.vertices[i].y = (x - cx) * sin(degree * M_PI/180) + (y - cy) * cos(degree * M_PI/180) + cy;
    }
}

void CShapes::scale2D(float sf)
{
    SVertex centroid = findCentroid2D();
    if(activeShape.vertices.size() > 3)
    {
        centroid = findCentroid3D();
    }
    float cx = centroid.x;
    float cy = centroid.y;
    float x = 0, y = 0;
    for(int i = 0; i < activeShape.vertices.size(); i++)
    {
        x = activeShape.vertices[i].x;
        y = activeShape.vertices[i].y;
        activeShape.vertices[i].x = (x - cx) * sf + cx;
        activeShape.vertices[i].y = (y - cy) * sf + cy;
    }
}

SVertex CShapes::findCentroid2D()
{
    SVertex centroid;
    centroid.set = false;
    if(activeShape.vertices.size() == 1)
    {
        centroid.x = activeShape.vertices[0].x;
        centroid.y = activeShape.vertices[0].y;
        centroid.set = true;
    }
    else if(activeShape.vertices.size() == 2)
    {
        centroid.x = (activeShape.vertices[0].x + activeShape.vertices[1].x) / 2;
        centroid.y = (activeShape.vertices[0].y + activeShape.vertices[1].y) / 2;
        centroid.set = true;
    }
    else if (activeShape.vertices.size() > 2)
    {
        float x = 0, y = 0, a = 0, ta = 0;
        int i = 0;
        for(i = 0; i < activeShape.vertices.size() - 1; i++)
        {
            ta = activeShape.vertices[i].x * activeShape.vertices[i+1].y - activeShape.vertices[i+1].x * activeShape.vertices[i].y;
            a += ta;
            x += (activeShape.vertices[i].x + activeShape.vertices[i+1].x) * ta;
            y += (activeShape.vertices[i].y + activeShape.vertices[i+1].y) * ta;
        }
        ta = activeShape.vertices[i].x * activeShape.vertices[0].y - activeShape.vertices[0].x * activeShape.vertices[i].y;
        a += ta;
        x += (activeShape.vertices[i].x + activeShape.vertices[0].x) * ta;
        y += (activeShape.vertices[i].y + activeShape.vertices[0].y) * ta;
        x /= 3 * a;
        y /= 3 * a;
        centroid.x = x;
        centroid.y = y;
        centroid.set = true;
    }
    return centroid;
}

void CShapes::clip2D()
{
    float zoom = CPixelBuffer::instance(0)->getZoom();
    SShape newShape;
    newShape.k = activeShape.k;
    newShape.knots = activeShape.knots;
    newShape.bezier = activeShape.bezier;
    SVertex newVertex;
    int pVI = 0;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < activeShape.vertices.size(); j++)
        {
            pVI = j - 1;
            if(j == 0){pVI = activeShape.vertices.size() - 1;}
            if((activeShape.vertices.size() != 2 && CPixelBuffer::instance(0)->ablrCode(activeShape.vertices[pVI],i) && !CPixelBuffer::instance(0)->ablrCode(activeShape.vertices[j],i)) || (!CPixelBuffer::instance(0)->ablrCode(activeShape.vertices[pVI],i) && CPixelBuffer::instance(0)->ablrCode(activeShape.vertices[j],i)))
            {
                switch(i)
                {
                    case 0:
                        newVertex = intersection(activeShape.vertices[pVI].x*zoom,activeShape.vertices[pVI].y*zoom,activeShape.vertices[j].x*zoom,activeShape.vertices[j].y*zoom, 0, CPixelBuffer::instance(0)->getTopView(), 1, CPixelBuffer::instance(0)->getTopView());
                        break;
                    case 1:
                        newVertex = intersection(activeShape.vertices[pVI].x*zoom,activeShape.vertices[pVI].y*zoom,activeShape.vertices[j].x*zoom,activeShape.vertices[j].y*zoom,0, CPixelBuffer::instance(0)->getBottomView(), 1, CPixelBuffer::instance(0)->getBottomView());
                        break;
                    case 2:
                        newVertex = intersection(activeShape.vertices[pVI].x*zoom,activeShape.vertices[pVI].y*zoom,activeShape.vertices[j].x*zoom,activeShape.vertices[j].y*zoom, CPixelBuffer::instance(0)->getLeftView(),0,CPixelBuffer::instance(0)->getLeftView(),1);
                        break;
                    case 3:
                        newVertex = intersection(activeShape.vertices[pVI].x*zoom,activeShape.vertices[pVI].y*zoom,activeShape.vertices[j].x*zoom,activeShape.vertices[j].y*zoom, CPixelBuffer::instance(0)->getRightView(),0,CPixelBuffer::instance(0)->getRightView(),1);
                        break;
                }
                if(newVertex.set)
                {
                    newVertex.x /= zoom;
                    newVertex.y /= zoom;
                    newShape.vertices.push_back(newVertex);
                }
            }
            if(!CPixelBuffer::instance(0)->ablrCode(activeShape.vertices[j],i)){newShape.vertices.push_back(activeShape.vertices[j]);}
        }
        activeShape = newShape;
        newShape.vertices.clear();
    }
}

SVertex CShapes::intersection(float x1,float y1,float x2,float y2,float x3,float y3,float x4, float y4)
{
    SVertex point;
    point.set = false;
    float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    float x = 0, y = 0;
    if(denom == 0)
    {
        return point;
    }
    else
    {
        x = round(((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3*y4-y3*x4)) / denom);
        y = round(((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3*y4-y3*x4)) / denom);
        point.x = (x);
        point.y = (y);
        point.set = true;
    }
    return point;
}

SVertex CShapes::findCentroid3D()
{
    SVertex centroid;
    centroid.set = false;
    if(activeShape.vertices.size() < 4){return centroid;}
    else{centroid.set = true;}
    centroid = activeShape.vertices[0];
    for(int i = 1; i < activeShape.vertices.size(); i++)
    {
        centroid.x += activeShape.vertices[i].x;
        centroid.y += activeShape.vertices[i].y;
        centroid.z += activeShape.vertices[i].z;
    }
    centroid.x /= activeShape.vertices.size();
    centroid.y /= activeShape.vertices.size();
    centroid.z /= activeShape.vertices.size();
    return centroid;
}

SVertex CShapes::findCentroid3D(SShape shape)
{
    SVertex centroid;
    centroid.set = false;
    if(shape.vertices.size() < 4){return centroid;}
    else{centroid.set = true;}
    centroid = shape.vertices[0];
    for(int i = 1; i < shape.vertices.size(); i++)
    {
        centroid.x += shape.vertices[i].x;
        centroid.y += shape.vertices[i].y;
        centroid.z += shape.vertices[i].z;
    }
    centroid.x /= shape.vertices.size();
    centroid.y /= shape.vertices.size();
    centroid.z /= shape.vertices.size();
    return centroid;
}

SVertex CShapes::findCentroid3D(STriangle triangle)
{
    SVertex centroid;
    centroid.set = false;
    centroid.set = true;
    centroid = triangle.vertices[0];
    for(int i = 1; i < 3; i++)
    {
        centroid.x += triangle.vertices[i].x;
        centroid.y += triangle.vertices[i].y;
        centroid.z += triangle.vertices[i].z;
    }
    centroid.x /= 3;
    centroid.y /= 3;
    centroid.z /= 3;
    return centroid;
}

void CShapes::translate3D( float x, float y, float z)
{
    for(int i = 0; i < activeShape.vertices.size(); i++)
    {
        activeShape.vertices[i].x += x;
        activeShape.vertices[i].y += y;
        activeShape.vertices[i].z += z;
    }
}

void CShapes::rotate3D( const SVertex p1, const SVertex p2, float degree)
{
    if(activeShape.vertices.size() < 4){return;}
    float unitVector[3] = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
    float unitVectorMag = sqrt(unitVector[0] * unitVector[0] + unitVector[1] * unitVector[1] + unitVector[2] * unitVector[2]);
    unitVector[0] /= unitVectorMag;
    unitVector[1] /= unitVectorMag;
    unitVector[2] /= unitVectorMag;
    float cosa = cos(degree / 180 * M_PI);
    float sina = sin(degree / 180 * M_PI);
    float rot[3][3];
    rot[0][0] = unitVector[0] * unitVector[0] * (1 - cosa) + cosa;
    rot[0][1] = unitVector[0] * unitVector[1] * (1 - cosa) - unitVector[2] * sina;
    rot[0][2] = unitVector[0] * unitVector[2] * (1 - cosa) + unitVector[1] * sina;
    rot[1][0] = unitVector[1] * unitVector[0] * (1 - cosa) + unitVector[2] * sina;
    rot[1][1] = unitVector[1] * unitVector[1] * (1 - cosa) + cosa;
    rot[1][2] = unitVector[1] * unitVector[2] * (1 - cosa) - unitVector[0] * sina;
    rot[2][0] = unitVector[2] * unitVector[0] * (1 - cosa) - unitVector[1] * sina;
    rot[2][1] = unitVector[2] * unitVector[1] * (1 - cosa) + unitVector[0] * sina;
    rot[2][2] = unitVector[2] * unitVector[2] * (1 - cosa) + cosa;
    for (int i = 0; i < activeShape.vertices.size(); i++) 
    {
        float tempP[3] = {activeShape.vertices[i].x - p1.x, activeShape.vertices[i].y - p1.y, activeShape.vertices[i].z - p1.z};
        float r[3] = {0};
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                r[j] += rot[j][k] * tempP[k];
        activeShape.vertices[i].x = r[0] + p1.x;
        activeShape.vertices[i].y = r[1] + p1.y;
        activeShape.vertices[i].z = r[2] + p1.z;
    }
}

void CShapes::scale3D(float scaleFactorX, float scaleFactorY, float scaleFactorZ)
{
    SVertex centroid = findCentroid3D();
    for(int i = 0; i < activeShape.vertices.size() ; i++)
    {
        activeShape.vertices[i].x -= centroid.x;
        activeShape.vertices[i].y -= centroid.y;
        activeShape.vertices[i].z -= centroid.z;
        activeShape.vertices[i].x *= scaleFactorX;
        activeShape.vertices[i].y *= scaleFactorY;
        activeShape.vertices[i].z *= scaleFactorZ;
        activeShape.vertices[i].x += centroid.x;
        activeShape.vertices[i].y += centroid.y;
        activeShape.vertices[i].z += centroid.z;
    }
}

void CShapes::calculateTriangleDepths()
{
    for(auto &triangle : activeShape.triangles)
    {
        triangle.vertices[0] = (activeShape.vertices[triangle.vertexIndices[0]]);
        triangle.vertices[1] = (activeShape.vertices[triangle.vertexIndices[1]]);
        triangle.vertices[2] = (activeShape.vertices[triangle.vertexIndices[2]]);

        triangle.depthX = (triangle.vertices[0].x < triangle.vertices[1].x) ? ((triangle.vertices[0].x < triangle.vertices[2].x) ? triangle.vertices[0].x : triangle.vertices[2].x) : ((triangle.vertices[1].x < triangle.vertices[2].x) ? triangle.vertices[1].x : triangle.vertices[2].x);
        triangle.depthY = (triangle.vertices[0].y < triangle.vertices[1].y) ? ((triangle.vertices[0].y < triangle.vertices[2].y) ? triangle.vertices[0].y : triangle.vertices[2].y) : ((triangle.vertices[1].y < triangle.vertices[2].y) ? triangle.vertices[1].y : triangle.vertices[2].y);
        triangle.depthZ = (triangle.vertices[0].z < triangle.vertices[1].z) ?  ((triangle.vertices[0].z < triangle.vertices[2].z) ? triangle.vertices[0].z : triangle.vertices[2].z) : ((triangle.vertices[1].z < triangle.vertices[2].z) ? triangle.vertices[1].z : triangle.vertices[2].z);
        triangle.depthX += findCentroid3D(triangle).x;
        triangle.depthY += findCentroid3D(triangle).y;
        triangle.depthZ += findCentroid3D(triangle).z;
    }
    for(auto &shape : allShapes)
    {
        for(auto &triangle : shape.triangles)
        {
            triangle.vertices[0] = (shape.vertices[triangle.vertexIndices[0]]);
            triangle.vertices[1] = (shape.vertices[triangle.vertexIndices[1]]);
            triangle.vertices[2] = (shape.vertices[triangle.vertexIndices[2]]);

            triangle.depthX = (triangle.vertices[0].x < triangle.vertices[1].x) ? ((triangle.vertices[0].x < triangle.vertices[2].x) ? triangle.vertices[0].x : triangle.vertices[2].x) : ((triangle.vertices[1].x < triangle.vertices[2].x) ? triangle.vertices[1].x : triangle.vertices[2].x);
            triangle.depthY = (triangle.vertices[0].y < triangle.vertices[1].y) ? ((triangle.vertices[0].y < triangle.vertices[2].y) ? triangle.vertices[0].y : triangle.vertices[2].y) : ((triangle.vertices[1].y < triangle.vertices[2].y) ? triangle.vertices[1].y : triangle.vertices[2].y);
            triangle.depthZ = (triangle.vertices[0].z < triangle.vertices[1].z) ? ((triangle.vertices[0].z < triangle.vertices[2].z) ? triangle.vertices[0].z : triangle.vertices[2].z) : ((triangle.vertices[1].z < triangle.vertices[2].z) ? triangle.vertices[1].z : triangle.vertices[2].z);
            triangle.depthX += findCentroid3D(triangle).x;
            triangle.depthY += findCentroid3D(triangle).y;
            triangle.depthZ += findCentroid3D(triangle).z;
        }
    }
}

void swap(SVertex & p1, SVertex & p2)
{
    SVertex temp = p2;
    p2 = p1;
    p1 = temp;
}

bool CShapes::depthSortFunctionX(STriangle t1, STriangle t2)
{
    return(t1.depthX < t2.depthX);
}
bool CShapes::depthSortFunctionY(STriangle t1, STriangle t2)
{
    return(t1.depthY < t2.depthY);
}
bool CShapes::depthSortFunctionZ(STriangle t1, STriangle t2)
{
    return(t1.depthZ < t2.depthZ);
}

std::vector <STriangle> CShapes::sortTrianglesByDepthX()
{
    SPoint viewPointYZ(999999, 0.5, 0.5);
    std::vector<STriangle> sortedTriangles;
    if(allShapes.size() > 0)
    {
        calculateVertexNormals();
        calculateTriangleDepths();
        for(auto shape : allShapes)
        {
            for(auto triangle : shape.triangles)
            {
                if(triangle.triangleNormal.dot(viewPointYZ) >= 0)
                {
                    sortedTriangles.push_back(triangle);
                }
            }
        }
        for(auto triangle : activeShape.triangles)
        {
            if(triangle.triangleNormal.dot(viewPointYZ) >= 0)
            {
                sortedTriangles.push_back(triangle);
            }  
        }
        sort(sortedTriangles.begin(), sortedTriangles.end(), depthSortFunctionX);
    }
    return sortedTriangles;
}

std::vector <STriangle> CShapes::sortTrianglesByDepthY()
{
    SPoint viewPointZX(0.5, 999999, 0.5);
    std::vector<STriangle> sortedTriangles;
    if(allShapes.size() > 0)
    {
        calculateVertexNormals();
        calculateTriangleDepths();
        for(auto shape : allShapes)
        {
            for(auto triangle : shape.triangles)
            {
                if(triangle.triangleNormal.dot(viewPointZX) >= 0)
                {
                    sortedTriangles.push_back(triangle);
                }
            }
        }
        for(auto triangle : activeShape.triangles)
        {
            if(triangle.triangleNormal.dot(viewPointZX) >= 0)
            {
                sortedTriangles.push_back(triangle);
            }  
        }
        sort(sortedTriangles.begin(), sortedTriangles.end(), depthSortFunctionY);
    }
    return sortedTriangles;
}

std::vector <STriangle> CShapes::sortTrianglesByDepthZ()
{
    SPoint viewPointXY(0.5, 0.5, 999999);
    std::vector<STriangle> sortedTriangles;
    if(allShapes.size() > 0)
    {
        calculateVertexNormals();
        calculateTriangleDepths();
        for(auto shape : allShapes)
        {
            for(auto triangle : shape.triangles)
            {
                if(triangle.triangleNormal.dot(viewPointXY) >= 0)
                {
                    sortedTriangles.push_back(triangle);
                }
            }
        }
        for(auto triangle : activeShape.triangles)
        {
            if(triangle.triangleNormal.dot(viewPointXY) >= 0)
            {
                sortedTriangles.push_back(triangle);
            }  
        }
        sort(sortedTriangles.begin(), sortedTriangles.end(), depthSortFunctionZ);
    }
    return sortedTriangles;
}

void CShapes::calculateVertexNormals()
{
    for(auto & tshape : allShapes)
    {
        calculateVertexNormals(tshape);
    }
    calculateVertexNormals(activeShape);
}

void CShapes::calculateVertexNormals(SShape &shape)
{
    SPoint center = findCentroid3D(shape);
    for (int j = 0; j < shape.vertices.size(); j++)
    {
        shape.vertices[j].normal = SPoint();
        for (int k = 0; k < shape.triangles.size(); k++)
        {
            if (shape.triangles[k].vertexIndices[0] == j || shape.triangles[k].vertexIndices[1] == j || shape.triangles[k].vertexIndices[2] == j)
            {
                SPoint normal = (shape.vertices[shape.triangles[k].vertexIndices[0]] - shape.vertices[shape.triangles[k].vertexIndices[1]]).cross(shape.vertices[shape.triangles[k].vertexIndices[1]] - shape.vertices[shape.triangles[k].vertexIndices[2]]);
                if ((shape.vertices[shape.triangles[k].vertexIndices[0]] - center).normalize().dot(normal.normalize()) < 0) normal = normal *  -1;
                shape.triangles[k].triangleNormal = normal;
                shape.vertices[j].normal = shape.vertices[j].normal + normal;
            }
        }
        shape.vertices[j].normal.normalize();
    }
}

void CShapes::projectTriangle(STriangle& triangle)
{
    for(int i = 0; i < 3; i++)
    {
        triangle.zxProj[i] = triangle.yzProj[i] = triangle.xyProj[i] =  triangle.vertices[i];
        triangle.xyProj[i].x = triangle.vertices[i].x;
        triangle.xyProj[i].y = triangle.vertices[i].y;
        triangle.yzProj[i].x = triangle.vertices[i].y;
        triangle.yzProj[i].y = triangle.vertices[i].z;
        triangle.zxProj[i].x = triangle.vertices[i].z;
        triangle.zxProj[i].y = triangle.vertices[i].x;
    }
    if (triangle.xyProj[0].y > triangle.xyProj[1].y) swap(triangle.xyProj[0], triangle.xyProj[1]);
    if (triangle.xyProj[0].y > triangle.xyProj[2].y) swap(triangle.xyProj[0], triangle.xyProj[2]);
    if (triangle.xyProj[1].y > triangle.xyProj[2].y) swap(triangle.xyProj[1], triangle.xyProj[2]);
    if (triangle.yzProj[0].y > triangle.yzProj[1].y) swap(triangle.yzProj[0], triangle.yzProj[1]);
    if (triangle.yzProj[0].y > triangle.yzProj[2].y) swap(triangle.yzProj[0], triangle.yzProj[2]);
    if (triangle.yzProj[1].y > triangle.yzProj[2].y) swap(triangle.yzProj[1], triangle.yzProj[2]);
    if (triangle.zxProj[0].y > triangle.zxProj[1].y) swap(triangle.zxProj[0], triangle.zxProj[1]);
    if (triangle.zxProj[0].y > triangle.zxProj[2].y) swap(triangle.zxProj[0], triangle.zxProj[2]);
    if (triangle.zxProj[1].y > triangle.zxProj[2].y) swap(triangle.zxProj[1], triangle.zxProj[2]);
}