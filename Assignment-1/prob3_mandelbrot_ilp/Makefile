CXX=g++ -m64
CXXFLAGS=-I../common -Iobjs/ -O3 -Wall -mavx2

APP_NAME=mandelbrot
OBJDIR=objs
COMMONDIR=../common

PPM_CXX=$(COMMONDIR)/ppm.cpp
PPM_OBJ=$(addprefix $(OBJDIR)/, $(subst $(COMMONDIR)/,, $(PPM_CXX:.cpp=.o)))

CALLIBRATE_CXX=$(COMMONDIR)/callibrate.cpp
CALLIBRATE_OBJ=$(addprefix $(OBJDIR)/, $(subst $(COMMONDIR)/,, $(CALLIBRATE_CXX:.cpp=.o)))


default: $(APP_NAME) mandelbrot.s

.PHONY: dirs clean

dirs:
		/bin/mkdir -p $(OBJDIR)/

clean:
		/bin/rm -rf $(OBJDIR) *.o mandelbrot.s *.ppm *~ $(APP_NAME)

OBJS=$(OBJDIR)/main.o $(OBJDIR)/mandelbrot.o $(PPM_OBJ) $(CALLIBRATE_OBJ)

$(APP_NAME): dirs $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -lm 

$(OBJDIR)/%.o: %.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@

%.s: %.cpp
		$(CXX) $< $(CXXFLAGS) -S -o $@


$(OBJDIR)/%.o: $(COMMONDIR)/%.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/main.o: $(COMMONDIR)/CycleTimer.h mandelbrot.h 


