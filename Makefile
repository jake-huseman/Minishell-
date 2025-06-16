TARGET = main
CC = gcc
CXX = g++
CFLAGS = -Wall -g 

OBJECTS = main.o

all: $(TARGET)
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c $< -o $@
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $@

clean:
	rm -f $(TARGET) *.o core *.core.* vgcore.*