CXX=g++
CXXFLAGS=-g `pkg-config opencv4 --cflags`
LDLIBS= -lglut -lGL -lGLU `pkg-config opencv4 --libs`
PRG = main
OBJ = main.o cv.o

$(PRG) : $(OBJ)
	$(CXX) -o $@ $^ $(LDLIBS)

.c.o:
	$(CXX) $(CXXFLAGS) -c $^

clean: 
	rm *.o