OBJECTS =  Color.o PixelBuffer.o Shapes.o GraphLib.o FileIO.o Main.o
TARGET = GLib
all: $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) -lglut -lGL
PixelBuffer.o: PixelBuffer.cpp PixelBuffer.h Color.h Shapes.h
	g++ -c $<
Color.o: Color.cpp Color.h
	g++ -c $<
GraphLib.o: GraphLib.cpp GraphLib.h Color.h PixelBuffer.h Shapes.h
	g++ -c $<
Shapes.o: Shapes.cpp Shapes.h Color.h GraphLib.h PixelBuffer.h
	g++ -c $<
FileIO.o: FileIO.cpp FileIO.h Shapes.h
	g++ -c $<
Main.o: Main.cpp PixelBuffer.h Color.h GraphLib.h Shapes.h FileIO.h
	g++ -c $<
clean: 
	rm $(TARGET) $(OBJECTS) 
