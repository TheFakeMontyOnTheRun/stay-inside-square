CXXFLAGS = -g -c -Ofast -ffast-math -mtune=i486 -march=i486

CXX = i586-pc-msdosdjgpp-g++

OBJS = demo.o


TARGET = demo.exe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm *~
