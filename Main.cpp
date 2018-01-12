#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <limits>
#include "Color.h"
#include "PixelBuffer.h"
#include "GraphLib.h"
#include "FileIO.h"
#include "Shapes.h"

// Window Size.
int winSizeH = 500;
int winSizeV = 500;

// Window ID.
int MainWindow;
int xyWindow;
int zxWindow;
int yzWindow;

// Mode bools.
bool _3Dmode = false;
bool terminalMode = false;
bool curveEditor = false;

// Line draw algorithm for polygons.
// Toggles between Bresenham and DDA.
bool useDDA = false;

// Whether to fill in polygons.
bool bFill = true;
// Whether to draw the border for polygons.
bool drawOutline = true;

// Switch how to manipulate an object.
bool translate = true;
bool rotate = false;
bool scale = false;

// Modify the viewport.
bool viewportMove = false;
bool borderSizeChange = false;

// Used when rotating a 3D object using terminal commands.
bool rotationLineInput = false;
SVertex rotStartPt;
SVertex rotEndPt;
float rotDegree = 0;

// Whether to move the light source or not.
bool lightSource = false;

// Whether to modify a vertex.
bool modifyingVertex = false;
int indexOfVertexToModify = -1;

// B-spline curve order.
int curveOrder = 2;
int curveResolution = 1000;

CColor backgroundColor (.2,.2,.2);

// Viewpoint for each viewport in 3D mode.
SPoint viewPointXY(0.5, 0.5, 5);
SPoint viewPointYZ(5, 0.5, 0.5);
SPoint viewPointZX(0.5, 5, 0.5);

// 2D
void display2D();
// Inputs.
void mouseClick2D(int button, int state, int x, int y);
void mouseDragVertex(int x, int y);
void keyActions2D(unsigned char key, int x, int y);
void specialInput2D(int key, int x, int y);

// 3D
void refreshAllWindows();
void display3DXY();
void display3DZX();
// Inputs.
void display3DYZ();
void keyActions3D(unsigned char key, int x, int y);
void specialInput3D(int key, int x, int y);
void consoleInput();
void timer(int val);

int main(int argc, char *argv[])
{
    // Choose the mode the program will start in.
    std::string userInput = "";
    std::cout << "Currently in 2D mode. Do you want to switch to 3D mode? y/n: ";
    std::cin >> userInput;
    if(userInput == "y" || userInput == "Y")
    {
        std::cout << "3D mode selected" << std::endl;
        _3Dmode = true;
        curveEditor = false;
        std::cout << "Do you want to use the terminal to input commands? y/n: ";
        std::cin >> userInput;
        if(userInput == "y" || userInput == "Y")
        {
            std::cout << "Terminal mode selected." << std::endl;
            terminalMode = true;
        }
        else 
        {
            std::cout <<
            "\nt         - translate mode; use arrow keys to move the active object\n"<<
            "r         - rotate mode; use the arrow keys to rotate the active object\n"<<
            "s         - scale mode; use arrow keys to scale the object; hold SHIFT to only scale in one direction\n"<<
            "w         - toggle wireframe mode\n"<<
            "h         - toggle half-tone mode\n"<<
            "z         - zoom out to fit all objects\n"<<
            "ALT+arrow - change active object\n"<<
            "SHIFT+s   - save changes to file\n" << std::endl;
        }
    }
    else
    {
        std::cout << "2D mode selected" << std::endl;
        _3Dmode = false;
        curveEditor = false;
        std::cout << "Currently in polygon mode. Do you want to switch to curve mode? y/n: ";
        std::cin >> userInput;
        if(userInput == "y" || userInput == "Y")
        {
            std::cout << "Curve mode selected." << std::endl << std::endl;
            std::cout <<
            "t         - translate mode; use arrow keys to move the active curve\n"<<
            "r         - rotate mode; use the arrow keys to rotate the active curve\n"<<
            "s         - scale mode; use arrow keys to scale the curve\n"<<
            ".         - increase k (b-spline order)\n"<<
            ",         - decrease k\n"<<
            "]         - increase resolution\n"<<
            "[         - decrease resolution\n"<<
            "/         - toggle between bezier and b-spline\n"<<
            "k         - edit knots for b-spline (prompts in terminal)\n"<<
            "v         - viewport move mode; use arrow keys to move the viewport\n"<<
            "b         - viewport scale mode; use arrow keys to scale the viewport\n"<<
            "LMB       - add a vertex to the active curve\n"<<
            "u         - undo the last vertex added\n"<<
            "SHIFT+u   - redo the last vertex removed by undoing\n"<<
            "SHIFT+LMB - drag mouse to move the closest vertex to the cursor\n"<<
            "CTRL+LMB  - insert a vertex between the closest two vertices\n"<<
            "CTRL+RMB  - delete the vertex closest to the cursor\n"<<
            "RMB       - can start making a new curve\n"<<
            "ALT+arrow - change active curve\n"<<
            "SHIFT+s   - save changes to file\n" << std::endl;
            curveEditor = true;
        }
        else
        {
            std::cout << "Polygon mode selected." << std::endl << std::endl;
            std::cout <<
            "l         - toggle between DDA(blue) and Bresenham(green) line drawing algorithms\n"<<
            "f         - toggle whether to fill in polygons or not\n"<<
            "o         - toggle whether to draw the outline or not\n"<<
            "t         - translate mode; use arrow keys to move the active polygon\n"<<
            "r         - rotate mode; use the arrow keys to rotate the active polygon\n"<<
            "s         - scale mode; use arrow keys to scale the polygon\n"<<
            "c         - clip the polygon if it's partially outside the viewport\n"<<
            "v         - viewport move mode; use arrow keys to move the viewport\n"<<
            "b         - viewport scale mode; use arrow keys to scale the viewport\n"<<
            "LMB       - add a vertex to the active shape\n"<<
            "u         - undo the last vertex added\n"<<
            "SHIFT+u   - redo the last vertex removed by undoing\n"<<
            "SHIFT+LMB - drag mouse to move the closest vertex to the cursor\n"<<
            "CTRL+LMB  - insert a vertex between the closest two vertices\n"<<
            "CTRL+RMB  - delete the vertex closest to the cursor\n"<<
            "RMB       - can start making a new polygon\n"<<
            "ALT+arrow - change active polygon\n"<<
            "SHIFT+s   - save changes to file\n" << std::endl;
        }
    }
    
    // Setup.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(winSizeH, winSizeV);
    if(!_3Dmode)
    {
        glutInitWindowPosition(0, 0);
        MainWindow = glutCreateWindow("2D Mode");
        glClearColor(0, 0, 0, 0);
        glutDisplayFunc(display2D);
        glutMouseFunc(mouseClick2D);
        glutMotionFunc(mouseDragVertex);
        glutKeyboardFunc(keyActions2D);
        glutSpecialFunc(specialInput2D);
        CPixelBuffer::instance(0)->setPixelBufferSize(winSizeH,winSizeV, 10, 10, 10, 10, "xy");
    }
    else
    {
        glutInitWindowPosition(0, 0);
        xyWindow = glutCreateWindow("XY");
        glClearColor(0, 0, 0, 0);
        glutDisplayFunc(display3DXY);
        glutKeyboardFunc(keyActions3D);
        glutSpecialFunc(specialInput3D);

        glutInitWindowPosition(winSizeH + 8, 0);
        yzWindow = glutCreateWindow("YZ");
        glClearColor(0, 0, 0, 0);
        glutDisplayFunc(display3DYZ);
        glutKeyboardFunc(keyActions3D);
        glutSpecialFunc(specialInput3D);

        glutInitWindowPosition(winSizeH*2 + 16, 0);
        zxWindow = glutCreateWindow("ZX");
        glClearColor(0, 0, 0, 0);
        glutDisplayFunc(display3DZX);
        glutKeyboardFunc(keyActions3D);
        glutSpecialFunc(specialInput3D);

        CPixelBuffer::instance(xyWindow)->setPixelBufferSize(winSizeH,winSizeV, 0, 0, 0, 0, "xy");
        CPixelBuffer::instance(yzWindow)->setPixelBufferSize(winSizeH,winSizeV, 0, 0, 0, 0, "yz");
        CPixelBuffer::instance(zxWindow)->setPixelBufferSize(winSizeH,winSizeV, 0, 0, 0, 0, "zx");
    }

    // Load shapes from files.
    CFileIO::loadFile(_3Dmode, curveEditor);

    if(terminalMode)
    {
        //glutIdleFunc(consoleInput);
        glutTimerFunc(100, timer, 0);
    }

	glutMainLoop();
	return 0;
}

void timer( int value )
{
    consoleInput();
    refreshAllWindows();
    glutTimerFunc(100, timer, 0 );
}

void refreshAllWindows()
{
    glutPostWindowRedisplay(xyWindow);
    glutPostWindowRedisplay(yzWindow);
    glutPostWindowRedisplay(zxWindow);
}

// Display function for 2D mode.
void display2D()
{   
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

    // Clearing the pixel buffer.
    CPixelBuffer::instance(0)->resetPixelBuffer(backgroundColor);
    
    if(!curveEditor)
    {
        // Draw polygons.
        CShapes::instance()->drawAllShapes2D(drawOutline, useDDA, bFill);
    }
    else
    {
        // Making sure order and knot values are correct for drawing curves.
        if(CShapes::instance()->getActiveShape().k > CShapes::instance()->getActiveShape().vertices.size())
        {
            CShapes::instance()->setK(CShapes::instance()->getActiveShape().vertices.size());
        }
        if(CShapes::instance()->getActiveShape().knots.size() < CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k)
        {
            CShapes::instance()->addKnot(0);
        }
        // Draw curves.
        CShapes::instance()->drawAllCurves(curveResolution);
    }
    // Display the pixels in the buffer, then switch the buffer.
	glDrawPixels(winSizeH, winSizeV, GL_RGB, GL_FLOAT, CPixelBuffer::instance(0)->getPixelBuffer());
	glutSwapBuffers();
}

// Function used for moving the vertex closest to the cursor.
void mouseDragVertex(int x, int y)
{
    if(modifyingVertex)
    {
        SVertex point;
        point.x = (float)x / (float)winSizeH;
        point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
        CShapes::instance()->modifyVertexActiveShape(point, indexOfVertexToModify);
        glutPostRedisplay();
    }
}

// Mouse input for 2D mode.
void mouseClick2D(int button, int state, int x, int y)
{
    // Checks if shift is being held down.
    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT)
    {
        // On left click it moves the closest vertex to the cursor.
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            SVertex point;
            point.x = (float)x / (float)winSizeH;
            point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
            float minDist = 100000;
            float distTemp = 0;
            int i = 0;
            int indexToChange = -1;
            for(const auto vertex : CShapes::instance()->getActiveShape().vertices)
            {
                distTemp = point.distance(vertex);
                if(distTemp < minDist)
                {
                    minDist = distTemp;
                    indexToChange = i;
                }
                i++;
            }
            CShapes::instance()->modifyVertexActiveShape(point, indexToChange);
            glutPostRedisplay();
            indexOfVertexToModify = indexToChange;
            modifyingVertex = true;
        }
    }
    // If Ctrl key is being held down.
    else if(glutGetModifiers() == GLUT_ACTIVE_CTRL)
    {
        // On right click, delete the vertex closest to the cursor.
        if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            SVertex point;
            point.x = (float)x / (float)winSizeH;
            point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
            float minDist = 100000;
            float distTemp = 0;
            int i = 0;
            int indexToChange = -1;
            for(const auto vertex : CShapes::instance()->getActiveShape().vertices)
            {
                distTemp = point.distance(vertex);
                if(distTemp < minDist)
                {
                    minDist = distTemp;
                    indexToChange = i;
                }
                i++;
            }
            CShapes::instance()->deleteVertexActiveShape(indexToChange);
            glutPostRedisplay();
        }
        // On left click, add a vertex between the closest two vertices. 
        else if(CShapes::instance()->getActiveShape().vertices.size() > 1 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            SVertex point;
            point.x = (float)x / (float)winSizeH;
            point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
            float minDist = 100000;
            float distTemp = 0;
            int indexToChange = -1;
            auto test = CShapes::instance()->getActiveShape().vertices;
            for(int i = 0; i < CShapes::instance()->getActiveShape().vertices.size(); i++)
            {
                distTemp = point.distanceToSegment(CShapes::instance()->getActiveShape().vertices[i], CShapes::instance()->getActiveShape().vertices[(i+1) % CShapes::instance()->getActiveShape().vertices.size()]);
                if(distTemp < minDist)
                {
                    minDist = distTemp;
                    indexToChange = i;
                }
            }
            CShapes::instance()->insertVertexActiveShape(point, indexToChange);
            glutPostRedisplay();
        }
    }
    else
    {
        // On left click, add a vertex to current shape.
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            SVertex point;
            point.x = (float)x / (float)winSizeH;
            point.y = (float)(glutGet(GLUT_WINDOW_HEIGHT) - y) / (float)winSizeV;
            CShapes::instance()->addVertexToActiveShape(point);
            if(curveEditor)
            {
                if(CShapes::instance()->getActiveShape().vertices.size() < 3)
                {
                    CShapes::instance()->setK(2);
                }
            }
            glutPostRedisplay();
        }
        // On right click, finish drawing shape.
        if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            CShapes::instance()->addActiveToList();
            glutPostRedisplay();
        }
        // Resetting some values when left click is not down.
        if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        {
            modifyingVertex = false;
            indexOfVertexToModify = -1;
        }
    }
}

// Keyboard inputs for 2D mode.
void keyActions2D(unsigned char key, int x, int y)
{
    // Toggle line drawing method;
    if(key == 'l')
    {
        useDDA = !useDDA;
        glutPostRedisplay();
    }
    // remove last added point
    else if(key == 'u')
    {
        CShapes::instance()->undoVertexAdd();
        glutPostRedisplay();
    }
    // re-add removed point
    else if(key == 'U')
    {
        CShapes::instance()->redoVertexAdd();
        glutPostRedisplay();
    }
    // toggle filling in shapes or not
    else if(key == 'f')
    {
        bFill = !bFill;
        glutPostRedisplay();
    }
    // toggle drawing outline
    else if(key == 'o')
    {
        drawOutline = !drawOutline;
        glutPostRedisplay();
    }
    // clip
    else if(key == 'c')
    {
        CShapes::instance()->clip2D();
        glutPostRedisplay();
    }
    // translate
    else if(key == 't')
    {
        translate = true;
        borderSizeChange = viewportMove = scale = rotate = false;
    }
    // rotate
    else if(key == 'r')
    {
        rotate = true;
        borderSizeChange = viewportMove = scale = translate = false;
    }
    // scale
    else if(key == 's')
    {
        scale = true;
        borderSizeChange = viewportMove = translate = rotate = false;
    }
    // save to file
    else if(key == 'S')
    {
        CFileIO::saveFile(_3Dmode, curveEditor);
        glutPostRedisplay();
    }
    // move viewport
    else if(key == 'v')
    {
        viewportMove = true;
        borderSizeChange = scale = translate = rotate = false;
    }
    // scale viewport
    else if(key == 'b')
    {
        borderSizeChange = true;
        viewportMove = scale = translate = rotate = false;
    }
    // increase curve order
    else if(key == '.')
    {
        curveOrder++;
        int max = CShapes::instance()->getActiveShape().vertices.size();
        if(curveOrder > max){curveOrder = max;}
        CShapes::instance()->setK(curveOrder);
        glutPostRedisplay();
    }
    // decrease curve order
    else if(key == ',')
    {
        curveOrder--;
        if(curveOrder < 2){curveOrder = 2;}
        CShapes::instance()->setK(curveOrder);
        glutPostRedisplay();
    }
    // Toggle between Bezier and B-spline
    else if(key == '/')
    {
        CShapes::instance()->toggleBezier();
        glutPostRedisplay();
    }
    // Toggle between polygon mode and curve mode.
    else if(key == ';')
    {
        curveEditor = !curveEditor;
        glutPostRedisplay();
    }
    // Decrease resolution.
    else if(key == '[')
    {
        if(curveResolution >= 200)
        {
            curveResolution -= 100;
        }
        else if (curveResolution < 200 && curveResolution >= 20)
        {
            curveResolution -= 10;
        }
        else
        {
            curveResolution -= 1;
        }
        if (curveResolution < 2) {curveResolution = 2;}
        glutPostRedisplay();
    }
    // Increase resolution.
    else if(key == ']')
    {
        if(curveResolution >= 100)
        {
            curveResolution += 100;
        }
        else if (curveResolution < 100 && curveResolution >= 10)
        {
            curveResolution += 10;
        }
        else
        {
            curveResolution += 1;
        }
        glutPostRedisplay();
    }
    // Edit knots.
    else if(key == 'k' && !CShapes::instance()->getActiveShape().bezier)
    {
        std::cout << "Knots ([knot index]knot value): " << std::endl;
        for(int i=0; i < CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k; i++)
        {
            std::cout << "[" << i+1 << "]" << (float)CShapes::instance()->getActiveShape().knots[i];
            if(i < CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
        
        
        std::cout << "Which knot do you want to edit?[1-" << CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k <<"]: " << std::endl;
        std::string knotsIndex;
        int index = 0;
        std::cin >> knotsIndex;
        try
        {
            index = std::stoi(knotsIndex);
        }
        catch (std::invalid_argument){}
        while(index < 1 || index > CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k)
        {
            std::cout << "Invalid index. Please enter a valid index: " << std::endl;
            std::cin >> knotsIndex;
            try
            {
                index = std::stoi(knotsIndex);
            }
            catch (std::invalid_argument){}
        }
        float minKnot, maxKnot;
        if(index == 1)
        {
            minKnot = 0;
        }
        else
        {
            minKnot = CShapes::instance()->getActiveShape().knots[index - 2];
        }
        if(index == CShapes::instance()->getActiveShape().vertices.size() + CShapes::instance()->getActiveShape().k)
        {
            maxKnot = std::numeric_limits<float>::infinity();
        }
        else
        {
            maxKnot = CShapes::instance()->getActiveShape().knots[index];
        }
        std::cout << "Enter new value["<< minKnot << "-" << maxKnot <<"]: " << std::endl;
        std::string knotValue;
        float newKnotVal = -1;
        std::cin >> knotValue;
        try
        {
            newKnotVal = std::stof(knotValue);
        }
        catch (std::invalid_argument){}
        while(newKnotVal < minKnot || newKnotVal > maxKnot)
        {
            std::cout << "Invalid knot value. Please enter a valid value: " << std::endl;
            std::cin >> knotValue;
            try
            {
                newKnotVal = std::stof(knotValue);
            }
            catch (std::invalid_argument){}
        }
        CShapes::instance()->modKnot(index-1, newKnotVal);
        glutPostRedisplay();
    }
}

// Cursor keys used to either translate, rotate, scale, 
// move the viewport, or scale the viewport.
void specialInput2D(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
    {
        if(translate){CShapes::instance()->translate2D('u');}
        else if(viewportMove)
        {
            CPixelBuffer::instance(0)->changeBorder('u', -1);
            CPixelBuffer::instance(0)->changeBorder('d', 1);
        }
        else if(borderSizeChange)
        {
            CPixelBuffer::instance(0)->changeBorder('u', -1);
            CPixelBuffer::instance(0)->changeBorder('d', -1);
        }
    }
    else if(key == GLUT_KEY_DOWN)
    {
        if(translate){CShapes::instance()->translate2D('d');}
        else if(viewportMove)
        {
            CPixelBuffer::instance(0)->changeBorder('u', 1);
            CPixelBuffer::instance(0)->changeBorder('d', -1);
        }
        else if(borderSizeChange)
        {
            CPixelBuffer::instance(0)->changeBorder('u', 1);
            CPixelBuffer::instance(0)->changeBorder('d', 1);
        }
    }
    else if(key == GLUT_KEY_LEFT)
    {
        if(glutGetModifiers() == GLUT_ACTIVE_ALT)
        {
            CShapes::instance()->getPreviousShape();
        }
        else
        {
            if(translate){CShapes::instance()->translate2D('l');}
            else if(rotate){CShapes::instance()->rotate2D(-1);}
            else if(scale){CShapes::instance()->scale2D(1/1.1);}
            else if(viewportMove)
            {
                CPixelBuffer::instance(0)->changeBorder('l', -1);
                CPixelBuffer::instance(0)->changeBorder('r', 1);
            }
            else if(borderSizeChange)
            {
                CPixelBuffer::instance(0)->changeBorder('l', 1);
                CPixelBuffer::instance(0)->changeBorder('r', 1);
            }
        }
    }
    else if(key == GLUT_KEY_RIGHT)
    {
        if(glutGetModifiers() == GLUT_ACTIVE_ALT)
        {
            CShapes::instance()->getNextShape();
        }
        else
        {
            if(translate){CShapes::instance()->translate2D( 'r');}
            else if(rotate){CShapes::instance()->rotate2D( 1);}
            else if(scale){CShapes::instance()->scale2D( 1.1);}
            else if(viewportMove)
            {
                CPixelBuffer::instance(0)->changeBorder('l', 1);
                CPixelBuffer::instance(0)->changeBorder('r', -1);
            }
            else if(borderSizeChange)
            {
                CPixelBuffer::instance(0)->changeBorder('l', -1);
                CPixelBuffer::instance(0)->changeBorder('r', -1);
            }
        }
    }
    glutPostRedisplay();
}


// 3D display functions.
void display3DXY()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    CPixelBuffer::instance(xyWindow)->resetPixelBuffer(backgroundColor);
    CShapes::instance()->drawAllShapes3DXY(xyWindow, viewPointXY);

    // When using terminal commands to rotate, it displays the line around which
    // the 3D object will rotate.
    if(rotationLineInput)
    {
        float minCoord = CPixelBuffer::instance(xyWindow)->getMinCoord();
        SVertex startv;
        startv.x = -minCoord + rotStartPt.x;
        startv.y = -minCoord + rotStartPt.y;
        SVertex endv;
        endv.x = -minCoord + rotEndPt.x;
        endv.y = -minCoord + rotEndPt.y;
        CGraphLib::drawLineB(startv, endv,  CColor(1,0,0), xyWindow);
    }

    glDrawPixels(winSizeH, winSizeV, GL_RGB, GL_FLOAT, CPixelBuffer::instance(xyWindow)->getPixelBuffer());
    glutSwapBuffers();
}

void display3DYZ()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    CPixelBuffer::instance(yzWindow)->resetPixelBuffer(backgroundColor);
    CShapes::instance()->drawAllShapes3DYZ(yzWindow, viewPointYZ);

    if(rotationLineInput)
    {
        float minCoord = CPixelBuffer::instance(yzWindow)->getMinCoord();
        SVertex startv;
        startv.x = -minCoord + rotStartPt.y;
        startv.y = -minCoord + rotStartPt.z;
        SVertex endv;
        endv.x = -minCoord + rotEndPt.y;
        endv.y = -minCoord + rotEndPt.z;
        CGraphLib::drawLineB(startv, endv, CColor(1,0,0), yzWindow);
    }

    glDrawPixels(winSizeH, winSizeV, GL_RGB, GL_FLOAT, CPixelBuffer::instance(yzWindow)->getPixelBuffer());
    glutSwapBuffers();
}

void display3DZX()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    CPixelBuffer::instance(zxWindow)->resetPixelBuffer(backgroundColor);

    
    CShapes::instance()->drawAllShapes3DZX(zxWindow, viewPointZX);

    if(rotationLineInput)
    {
        float minCoord = CPixelBuffer::instance(zxWindow)->getMinCoord();
        SVertex startv;
        startv.x = -minCoord + rotStartPt.z;
        startv.y = -minCoord + rotStartPt.x;
        SVertex endv;
        endv.x = -minCoord + rotEndPt.z;
        endv.y = -minCoord + rotEndPt.x;
        CGraphLib::drawLineB(startv, endv, CColor(1,0,0), zxWindow);
    }

    glDrawPixels(winSizeH, winSizeV, GL_RGB, GL_FLOAT, CPixelBuffer::instance(zxWindow)->getPixelBuffer());
    glutSwapBuffers();
}

// Terminal commands.
void consoleInput()
{
    if(!rotationLineInput)
    {
        int count = 0;
        std::string userInput = "";
        std::string token = "";
        std::cout << "\nCommands:\nn     - next 3D shape\np     - prev 3D shape\nt     - translate\nr     - rotate\ns     - scale\nz     - zoom to fit\nht    - toggle half-tone mode\nwire  - toggle wireframe mode\nlight - move light source\niA    - change ambient light intensity\niL    - change light source intensity\nK     - change K\nphong - change Phong constant\nkA    - change Ambient constant\nkD    - change diffuse constant\nkS    - change specular constant\nq     - quit." << std::endl;
        std::cout << "Enter command:" << std::endl;
        std::cin >> userInput;
        if(userInput == "s")
        {
            std::cout << "Enter scale factor:" ;
            std::cin >> userInput;
            CShapes::instance()->scale3D(stof(userInput), stof(userInput), stof(userInput));
        }
        else if (userInput == "t")
        {
            float x = 0, y = 0, z = 0;
            std::cout << "Enter X value:" ;
            std::cin >> userInput;
            x = stof(userInput);
            std::cout << "Enter Y value:" ;
            std::cin >> userInput;
            y = stof(userInput);
            std::cout << "Enter Z value:" ;
            std::cin >> userInput;
            z = stof(userInput);
            CShapes::instance()->translate3D(x, y, z);
        }
        else if (userInput == "r")
        {
            float x = 0, y = 0, z = 0;
            float x2 = 0, y2 = 0, z2 = 0;
            float degree = 0;
            std::cout << "Enter X value for p1:" ;
            std::cin >> userInput;
            x = stof(userInput);
            std::cout << "Enter Y value for p1:" ;
            std::cin >> userInput;
            y = stof(userInput);
            std::cout << "Enter Z value for p1:" ;
            std::cin >> userInput;
            z = stof(userInput);
            std::cout << "Enter X value for p2:" ;
            std::cin >> userInput;
            x2 = stof(userInput);
            std::cout << "Enter Y value for p2:" ;
            std::cin >> userInput;
            y2 = stof(userInput);
            std::cout << "Enter Z value for p2:" ;
            std::cin >> userInput;
            z2 = stof(userInput);SVertex start;
            start.x = x;
            start.y = y;
            start.z = z;
            SVertex end;
            end.x = x2;
            end.y = y2;
            end.z = z2;
            std::cout << "Enter degrees:" ;
            std::cin >> userInput;
            degree = stof(userInput);
            rotationLineInput = true;
            rotStartPt = start;
            rotEndPt = end;
            rotDegree = degree;
        }
        else if (userInput == "n")
        {
            CShapes::instance()->getNextShape();
        }
        else if (userInput == "p")
        {
            CShapes::instance()->getPreviousShape();
        }
        else if (userInput == "z")
        {
            CPixelBuffer::instance(xyWindow)->changeZoom();
            CPixelBuffer::instance(yzWindow)->changeZoom();
            CPixelBuffer::instance(zxWindow)->changeZoom();
        }
        else if (userInput == "ht")
        {
            CGraphLib::toggleHalfTone();
        }
        else if(userInput == "wire")
        {
            CGraphLib::toggleWireframeMode();
        }
        else if(userInput == "light")
        {
            float x,y,z;
            std::cout << "Enter X value for light:" ;
            std::cin >> userInput;
            x = stof(userInput);
            std::cout << "Enter Y value for light:" ;
            std::cin >> userInput;
            y = stof(userInput);
            std::cout << "Enter Z value for light:" ;
            std::cin >> userInput;
            z = stof(userInput);
            CGraphLib::setLight(x, y, z);
        }
        else if(userInput == "iA")
        {
            float ia;
            std::cout << "Enter value for ambient light intensity:" ;
            std::cin >> userInput;
            ia = stof(userInput);
            CGraphLib::setAmbientLightIntensity(ia);
        }
        else if(userInput == "iL")
        {
            float il;
            std::cout << "Enter value for light source intensity:" ;
            std::cin >> userInput;
            il = stof(userInput);
            CGraphLib::setLightSourceIntensity(il);
        }
        else if(userInput == "K")
        {
            float k;
            std::cout << "Enter value for K:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setK(k);
        }
        else if(userInput == "phong")
        {
            float k;
            std::cout << "Enter value for Phong constant:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setPhongConstant(k);
        }
        else if(userInput == "kA")
        {
            float k;
            std::cout << "Enter value for Ambient constant:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setAmbientLightConstant(k);
        }
        else if(userInput == "kD")
        {
            float k;
            std::cout << "Enter value for Diffuse constant:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setDiffuseConstant(k);
        }
        else if(userInput == "kS")
        {
            float k;
            std::cout << "Enter value for Specular constant:" ;
            std::cin >> userInput;
            k = stof(userInput);
            CGraphLib::setSpecularConstant(k);
        }
        else if(userInput == "q")
        {
            exit(0);
        }
    }
    else
    {
        std::string userInput = "";
        std::cout << "Proceed with rotation? y/n:" ;
        std::cin >> userInput;
        if(userInput == "y" || userInput == "Y")
        {
            CShapes::instance()->rotate3D(rotStartPt, rotEndPt, rotDegree);
        }
        rotationLineInput = false;
    }
    refreshAllWindows();
    return;
}

// Keyboard input for 3D mode.
void keyActions3D(unsigned char key, int x, int y)
{
    // translate
    if(key == 't')
    {
        translate = true;
        scale = rotate = lightSource = false;
    }
    // rotate
    else if(key == 'r')
    {
        rotate = true;
        scale = translate = lightSource = false;
    }
    // scale
    else if(key == 's')
    {
        scale = true;
        translate = rotate = lightSource = false;
    }
    // Save file.
    else if(key == 'S')
    {
        CFileIO::saveFile(_3Dmode, curveEditor);
        refreshAllWindows();
    }
    // Zoom to fit all shapes.
    else if(key == 'z')
    {
        CPixelBuffer::instance(xyWindow)->changeZoom();
        CPixelBuffer::instance(yzWindow)->changeZoom();
        CPixelBuffer::instance(zxWindow)->changeZoom();
        refreshAllWindows();
    }
    // Toggle half tone mode.
    else if(key == 'h')
    {
        CGraphLib::toggleHalfTone();
        refreshAllWindows();
    }
    // Move the light source.
    else if (key == 'l')
    {
        lightSource = true;
        scale = translate = rotate = false;
    }
    // Toggle wireframe mode.
    else if(key == 'w')
    {
        CGraphLib::toggleWireframeMode();
        refreshAllWindows();
    }
}

// Setting up the cursor keys.
void specialInput3D(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
    {
        if(translate)
        {
            if(glutGetWindow() == xyWindow)
            {
                CShapes::instance()->translate3D( 0.0, 0.05, 0.0);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CShapes::instance()->translate3D( 0, 0, 0.05);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CShapes::instance()->translate3D( 0.05, 0, 0);
            }
        }
        else if(rotate)
        {
            SVertex pt1 = CShapes::instance()->findCentroid3D();
            SVertex pt2 = pt1;
            if(glutGetWindow() == xyWindow)
            {
                pt2.x += 0.5;
                CShapes::instance()->rotate3D( pt1, pt2, 5);
            }
            else if (glutGetWindow() == yzWindow)
            {
                pt2.y += 0.5;
                CShapes::instance()->rotate3D( pt1, pt2, 5);
            }
            else if (glutGetWindow() == zxWindow)
            {
                pt2.z += 0.5;
                CShapes::instance()->rotate3D( pt1, pt2, 5);
            }
        }
        else if(scale)
        {
            if(glutGetModifiers() != GLUT_ACTIVE_SHIFT)
            {
                CShapes::instance()->scale3D(1.5, 1.5, 1.5);
            }
            else if(glutGetWindow() == xyWindow)
            {
                CShapes::instance()->scale3D(1, 1.5, 1);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CShapes::instance()->scale3D( 1, 1, 1.5);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CShapes::instance()->scale3D( 1.5, 1, 1);
            }
        }
        else if(lightSource)
        {
            if(glutGetWindow() == xyWindow)
            {
                CGraphLib::moveLight(0, 0.05, 0);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CGraphLib::moveLight(0, 0, 0.05);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CGraphLib::moveLight(0.05, 0, 0);
            }
        }
    }
    else if(key == GLUT_KEY_DOWN)
    {
        if(translate)
        {
            if(glutGetWindow() == xyWindow)
            {
                CShapes::instance()->translate3D( 0.0, -0.05, 0.0);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CShapes::instance()->translate3D( 0, 0, -0.05);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CShapes::instance()->translate3D( -0.05, 0, 0);
            }
        }
        else if(rotate)
        {
            SVertex pt1 = CShapes::instance()->findCentroid3D();
            SVertex pt2 = pt1;
            if(glutGetWindow() == xyWindow)
            {
                pt2.x -= 0.5;
                CShapes::instance()->rotate3D(pt1, pt2, 5);
            }
            else if (glutGetWindow() == yzWindow)
            {
                pt2.y -= 0.5;
                CShapes::instance()->rotate3D(pt1, pt2, 5);
            }
            else if (glutGetWindow() == zxWindow)
            {
                pt2.z -= 0.5;
                CShapes::instance()->rotate3D(pt1, pt2, 5);
            }
        }
        else if(scale)
        {
            if(glutGetModifiers() != GLUT_ACTIVE_SHIFT)
            {
                CShapes::instance()->scale3D( 1/1.5, 1/1.5, 1/1.5);
            }
            else if(glutGetWindow() == xyWindow)
            {
                CShapes::instance()->scale3D( 1, 1/1.5, 1);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CShapes::instance()->scale3D( 1, 1, 1/1.5);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CShapes::instance()->scale3D( 1/1.5, 1, 1);
            }
        }
        else if(lightSource)
        {
            if(glutGetWindow() == xyWindow)
            {
                CGraphLib::moveLight(0, -0.05, 0);
            }
            else if (glutGetWindow() == yzWindow)
            {
                CGraphLib::moveLight(0, 0, -0.05);
            }
            else if (glutGetWindow() == zxWindow)
            {
                CGraphLib::moveLight(-0.05, 0, 0);
            }
        }
    }
    else if(key == GLUT_KEY_LEFT)
    {
        if(glutGetModifiers() == GLUT_ACTIVE_ALT)
        {
            CShapes::instance()->getPreviousShape();
        }
        else
        {
            if(translate)
            {
                if(glutGetWindow() == xyWindow)
                {
                    CShapes::instance()->translate3D( -0.05, 0, 0);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CShapes::instance()->translate3D(0.0, -0.05, 0.0);
                    
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CShapes::instance()->translate3D(0, 0, -0.05);
                }
            }
            else if(rotate)
            {
                SVertex pt1 = CShapes::instance()->findCentroid3D();
                SVertex pt2 = pt1;
                if(glutGetWindow() == xyWindow)
                {
                    pt2.y -= 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    pt2.z -= 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    pt2.x -= 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
            }
            else if(scale)
            {
                if(glutGetModifiers() != GLUT_ACTIVE_SHIFT)
                {
                    CShapes::instance()->scale3D(1/1.5, 1/1.5, 1/1.5);
                }
                else if(glutGetWindow() == xyWindow)
                {
                    CShapes::instance()->scale3D(1/1.5, 1, 1);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CShapes::instance()->scale3D(1, 1/1.5, 1);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CShapes::instance()->scale3D(1, 1, 1/1.5);
                }
            }
            else if(lightSource)
            {
                if(glutGetWindow() == xyWindow)
                {
                    CGraphLib::moveLight(-0.05, 0, 0);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CGraphLib::moveLight(0, -0.05, 0);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CGraphLib::moveLight(0, 0, -0.05);
                }
            }
        }
    }
    else if(key == GLUT_KEY_RIGHT)
    {
        if(glutGetModifiers() == GLUT_ACTIVE_ALT)
        {
            CShapes::instance()->getNextShape();
        }
        else
        {
            if(translate)
            {
                if(glutGetWindow() == xyWindow)
                {
                    CShapes::instance()->translate3D(0.05, 0, 0);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CShapes::instance()->translate3D(0.0, 0.05, 0.0);
                    
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CShapes::instance()->translate3D(0, 0, 0.05);
                }
            }
            else if(rotate)
            {
                SVertex pt1 = CShapes::instance()->findCentroid3D();
                SVertex pt2 = pt1;
                if(glutGetWindow() == xyWindow)
                {
                    pt2.y += 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    pt2.z += 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    pt2.x += 0.5;
                    CShapes::instance()->rotate3D( pt1, pt2, 5);
                }
            }
            else if(scale)
            {
                if(glutGetModifiers() != GLUT_ACTIVE_SHIFT)
                {
                    CShapes::instance()->scale3D( 1.5, 1.5, 1.5);
                }
                else if(glutGetWindow() == xyWindow)
                {
                    CShapes::instance()->scale3D( 1.5, 1, 1);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CShapes::instance()->scale3D( 1, 1.5, 1);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CShapes::instance()->scale3D( 1, 1, 1.5);
                }
            }
            else if(lightSource)
            {
                if(glutGetWindow() == xyWindow)
                {
                    CGraphLib::moveLight(0.05, 0, 0);
                }
                else if (glutGetWindow() == yzWindow)
                {
                    CGraphLib::moveLight(0, 0.05, 0);
                }
                else if (glutGetWindow() == zxWindow)
                {
                    CGraphLib::moveLight(0, 0, 0.05);
                }
            }
        }
    }
    refreshAllWindows();
}