OBJECTS =  Color.o PixelBuffer.o Shapes.o GraphLib.o FileIO.o Main.o
TARGET = GLib
all: $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) -lglut -lGL -ggdb
PixelBuffer.o: PixelBuffer.cpp PixelBuffer.h Color.h Shapes.h
	g++ -c -ggdb $<
Color.o: Color.cpp Color.h
	g++ -c -ggdb $<
GraphLib.o: GraphLib.cpp GraphLib.h Color.h PixelBuffer.h Shapes.h
	g++ -c -ggdb $<
Shapes.o: Shapes.cpp Shapes.h Color.h GraphLib.h PixelBuffer.h
	g++ -c -ggdb $<
FileIO.o: FileIO.cpp FileIO.h Shapes.h
	g++ -c -ggdb $<
Main.o: Main.cpp PixelBuffer.h Color.h GraphLib.h Shapes.h FileIO.h
	g++ -c -ggdb $<
clean: 
	rm $(TARGET) $(OBJECTS)
