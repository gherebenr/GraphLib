#include "FileIO.h"
#include "Shapes.h"
#include <iostream>
#include <fstream>
#include <ctime>

// Function to load the file needed for the selected mode.
void CFileIO::loadFile(bool _3Dmode, bool curveMode)
{
    if(!_3Dmode)
    {
        if(!curveMode)
        {
            std::ifstream inputFile("2DPolygonData.txt");
            if(inputFile.is_open())
            {
                SVertex inputVertex;
                std::string line;
                if(getline (inputFile,line))
                {
                    // Get the number of shapes.
                    int numShapes = std::stoi(line);
                    int numVertices = 0;
                    // For each shape.
                    for(int j = 0; j < numShapes; j++)
                    {
                        inputFile >> line;
                        // Get the number of vertices.
                        numVertices = std::stoi(line);
                        // For each vertex.
                        for(int i = 0; i < numVertices; i++)
                        {
                            inputFile >> line;
                            // Get the x value.
                            inputVertex.x = std::stof(line);
                            inputFile >> line;
                            // Get the y value.
                            inputVertex.y = std::stof(line);
                            // Add the vertex to a shape.
                            CShapes::instance()->addVertexToActiveShape(inputVertex);
                        }
                        if(j < numShapes - 1)
                        {
                            // Add the shape to an array once all vertices have been added.
                            CShapes::instance()->addActiveToList();
                        }
                    }
                    std::cout << "File Loaded!" << std::endl;
                }
                inputFile.close();
            }
        }
        else
        {
            std::ifstream inputFile("2DCurveData.txt");
            if(inputFile.is_open())
            {
                SVertex inputVertex;
                std::string line;
                if(getline (inputFile,line))
                {
                    // Get the number of shapes.
                    int numShapes = std::stoi(line);
                    int numVertices = 0;
                    int k = 0;
                    for(int j = 0; j < numShapes; j++)
                    {
                        inputFile >> line;
                        // Get the number of vertices for the shape.
                        numVertices = std::stoi(line);
                        for(int i = 0; i < numVertices; i++)
                        {
                            inputFile >> line;
                            // Get the x value.
                            inputVertex.x = std::stof(line);
                            inputFile >> line;
                            // Get the y value.
                            inputVertex.y = std::stof(line);
                            // Add the vertex to the shape.
                            CShapes::instance()->addVertexToActiveShape(inputVertex);
                        }
                        inputFile >> line;
                        k = std::stoi(line);
                        // If the next line has a number bigger than 0,
                        // then the curve is a B-Spline.
                        if(k > 0)
                        {
                            // Set the order.
                            CShapes::instance()->setK(k);
                            CShapes::instance()->addKnot(0);
                            CShapes::instance()->setBezier(false);
                            // Add the knots.
                            for(int m = 0; m < k + numVertices; m++)
                            {
                                inputFile >> line;
                                CShapes::instance()->modKnot(m, std::stof(line));
                            }
                        }
                        // Otherwise the curve is a Bezier curve.
                        else
                        {
                            CShapes::instance()->setBezier(true);
                        }
                        if(j < numShapes - 1)
                        {
                            // Add the curve to an array.
                            CShapes::instance()->addActiveToList();
                        }
                    }
                    std::cout << "File Loaded!" << std::endl;
                }
                inputFile.close();
            }
        }
    }
    else
    {
        std::ifstream inputFile("3DData.txt");
        if(inputFile.is_open())
        {
            SVertex inputVertex;
            STriangle inputTriangle;
            std::string line;
            if(getline (inputFile,line))
            {
                // Get the number of 3D objects.
                int numShapes = std::stoi(line);
                int numVertices = 0;
                int numEdges = 0;
                int numTriangles = 0;
                for(int j = 0; j < numShapes; j++)
                {
                    float r = ((rand() * time(NULL)) % 1000)/1000.f;
                    float g = ((rand() * time(NULL)) % 1000)/1000.f;
                    float b = ((rand() * time(NULL)) % 1000)/1000.f;
                    CColor color = CColor(r,g,b);

                    inputFile >> line;
                    numVertices = std::stoi(line);
                    // Get values for each vertex and add the vertex to the shape.
                    for(int i = 0; i < numVertices; i++)
                    {
                        inputFile >> line;
                        inputVertex.x = (std::stof(line));
                        inputFile >> line;
                        inputVertex.y = (std::stof(line));
                        inputFile >> line;
                        inputVertex.z = (std::stof(line));
                        CShapes::instance()->addVertexToActiveShape(inputVertex);
                    }
                    inputFile >> line;
                    // Get the number of triangles.
                    numTriangles = std::stoi(line);
                    // Each triangle has three indices, which correspond to a vertex
                    // that makes up that triangle. Each triangle that makes up the 
                    // 3D objects keeps a vector of indices.
                    for(int i = 0; i < numTriangles; i++)
                    {
                        inputFile >> line;
                        inputTriangle.vertexIndices[0] = std::stof(line);
                        inputTriangle.vertices[0] = (CShapes::instance()->getActiveShape().vertices[inputTriangle.vertexIndices[0]]);
                        inputFile >> line;
                        inputTriangle.vertexIndices[1] = std::stof(line);
                        inputTriangle.vertices[1] = (CShapes::instance()->getActiveShape().vertices[inputTriangle.vertexIndices[1]]);
                        inputFile >> line;
                        inputTriangle.vertexIndices[2] = std::stof(line);
                        inputTriangle.vertices[2] = (CShapes::instance()->getActiveShape().vertices[inputTriangle.vertexIndices[2]]);
                        inputTriangle.defaultColor = color;
                        CShapes::instance()->addTriangleToActiveShape(inputTriangle);
                    }
                    if(j < numShapes - 1)
                    {
                        // Add the object to an array.
                       CShapes::instance()->addActiveToList();
                    }
                }
                std::cout << "File Loaded!" << std::endl;
            }
            inputFile.close();
        }
    }
}

// Function to save objects to file.
void CFileIO::saveFile(bool _3Dmode, bool curveMode)
{
    CShapes::instance()->addActiveToList();
    
    if(!_3Dmode)
    {
        if(!curveMode)
        {
            std::ofstream outputFile ("2DPolygonData.txt");
            if(outputFile.is_open())
            {
                // Write the number of shapes to file.
                outputFile << CShapes::instance()->getAllShapes().size() <<  "\n";
                // For each shape write the number of vertices, then the coordinates for each vertex.
                for(SShape  tShape : CShapes::instance()->getAllShapes())
                {
                    outputFile <<  "\n" << tShape.vertices.size() << "\n";
                    for(int i = 0; i < tShape.vertices.size(); i++)
                    {
                        outputFile.precision(3);
                        outputFile << std::fixed << tShape.vertices[i].x << "\t" << tShape.vertices[i].y << "\n";
                    }
                }
                outputFile.close();
                std::cout << "File Saved!" << std::endl;
            } 
            else
            {
                std::cout << "File Save ERROR!" << std::endl;
            }
        }
        else
        {
            std::ofstream outputFile ("2DCurveData.txt");
            if(outputFile.is_open())
            {
                // Write the number of curves.
                outputFile << CShapes::instance()->getAllShapes().size() <<  "\n";
                // For each curve write the number of vertices, then the coordinates for those vertices.
                // If the curve is a B-Spline, also write out the values for each knot.
                for(SShape  tShape : CShapes::instance()->getAllShapes())
                {
                    outputFile <<  "\n" << tShape.vertices.size() << "\n";
                    for(int i = 0; i < tShape.vertices.size(); i++)
                    {
                        outputFile.precision(3);
                        outputFile << std::fixed << tShape.vertices[i].x << "\t" << tShape.vertices[i].y << "\n";
                    }
                    if(!tShape.bezier)
                    {
                        outputFile << tShape.k << "\n";
                        for(int i = 0; i < tShape.vertices.size() + tShape.k; i++)
                        {
                            outputFile << tShape.knots[i] << "\t";
                        }
                        outputFile << "\n";
                    }
                    else
                    {
                        outputFile << 0 << "\n";
                    }
                }
                outputFile.close();
                std::cout << "File Saved!" << std::endl;
            }
            else
            {
                std::cout << "File Save ERROR!" << std::endl;
            }
        }
    }
    else
    {
        std::ofstream outputFile ("3DData.txt");
        if(outputFile.is_open())
        {
            // Write the number of objects.
            outputFile << CShapes::instance()->getAllShapes().size() <<  "\n";
            // For each object, write the number of vertices and the coordinates of those vertices.
            // Then write the number of triangles, and the indices of the vertices that make them up.
            for(SShape tShape : CShapes::instance()->getAllShapes())
            {
                outputFile <<  "\n" << tShape.vertices.size() << "\n";
                for(int i = 0; i < tShape.vertices.size(); i++)
                {
                    outputFile.precision(3);
                    outputFile << std::fixed << tShape.vertices[i].x << "\t" << tShape.vertices[i].y << "\t" << tShape.vertices[i].z << "\n";
                }
                outputFile << tShape.triangles.size() << "\n";
                for(int i = 0; i < tShape.triangles.size(); i++)
                {
                    outputFile.precision(0);
                    outputFile << std::fixed << tShape.triangles[i].vertexIndices[0] << "\t" << tShape.triangles[i].vertexIndices[1] << "\t" << tShape.triangles[i].vertexIndices[2]<< "\n";
                }
            }
            outputFile.close();
            std::cout << "File Saved!" << std::endl;
        }
        else
        {
            std::cout << "File Save ERROR!" << std::endl;
        }
    }
}