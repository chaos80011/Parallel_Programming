# Define compiler and flags
CXX = mpic++
CXXFLAGS = -O3 -Wall

# Define the target executable
TARGET = matmul

# Define the source files
SRCS = main.cc matrix_multiply.cc  # Include any additional source files if needed
OBJS = $(SRCS:.cc=.o)

# Default target: build the executable
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile the source files into object files
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program with srun, using the input data file
run: $(TARGET)
	srun --mpi=pmix -N $(N) ./$(TARGET) $(data_file)

