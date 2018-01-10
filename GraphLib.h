#include <vector>
#include <string>
#include "Shapes.h"
#ifndef GRAPHLIB_H
#define GRAPHLIB_H



class CColor;
class CGraphLib
{
public:
    static void drawLineDDA2D(SVertex start, SVertex end, CColor color);
    static void drawLineB2D(SVertex start, SVertex end, CColor color, int pixelBufferIndex);
    static void drawPoint2D(float x, float y, CColor color, int pixelBufferIndex);
    static void fillShape2D(CColor color,SShape shape, int window);
    static CColor maxColor(const std::vector<STriangle>& allTriangles);
    static void calculateLightAtVertices(std::vector<STriangle>& allTriangles,SPoint viewPoint);
    static void calculateTriangleLight(STriangle& triangle,SPoint viewPoint);
    static void drawTriangle(STriangle& triangle, int window, std::string plane);
    static void edgeInterp(SVertex tVert[], int window, std::string plane, bool active);
    static void scanLineInterp(int startX, int endX, int y, CColor startC, CColor endC, int window);
    static void drawPoint3D(float x, float y, CColor color, int pixelBufferIndex);
    static void drawLightSource(std::string plane, int window);
    static void toggleHalfTone(){halfTone = !halfTone;}
    static void moveLight(float x, float y, float z);
    static void setLight(float x, float y, float z);
    static void setAmbientLightIntensity(float ia){iA = ia;}
    static void setLightSourceIntensity(float il){iL = il;}
    static void setK(float k){K = k;}
    static void setPhongConstant(float p){phongConst = p;}
    static void setAmbientLightConstant(float ka){kA = ka;}
    static void setDiffuseConstant(float kd){kD = kd;}
    static void setSpecularConstant(float ks){kS = ks;}
private:
    static float iA, iL, K, phongConst;
    static float kA, kD, kS;
    static SPoint lightSource;
    static bool halfTone;
};

#endif