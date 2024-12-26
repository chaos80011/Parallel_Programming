CXX = g++
CXXFLAGS = -Wall -O3 -march=native

TARGET = pi.out

SRC = pi.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
