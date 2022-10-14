APP_NAME=wireroute

OBJS=wireroute.o

CXX = g++ -m64 -std=c++11
CXXFLAGS = -I. -O3 -g -pg -Wall -fopenmp -Wno-unknown-pragmas -mavx2 -mfma -ffast-math -funroll-all-loops

default: $(APP_NAME)

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	/bin/rm -rf *~ *.o $(APP_NAME) *.class
