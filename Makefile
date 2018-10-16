CXXFLAGS = -g -c

CXX = ia16-elf-g++

OBJS = demo.o


TARGET = demo.com

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm *~
