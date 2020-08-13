default: run

PROGRAM = predict
SRCFILES = predict.cpp test_bench.cpp
HDRFILES = predict.h 
MODELFILES = vms_const.h vms_tb.h

OBJFILES := ${SRCFILES:.cpp=.o} 

DEPS = $(HDRFILES) $(MODELFILES) Makefile 

PGCXX = pgc++
CXXFLAGS += -g -std=c++11
CXXFLAGS += -fast -Minfo=accel -ta=tesla

run: $(PROGRAM)
	./$(PROGRAM) $(ARGS)

$(PROGRAM): $(OBJFILES)
	        $(PGCXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp $(DEPS)
	        $(PGCXX) $(CXXFLAGS)  -c -o $@ $<

.PHONY: clean

clean: 
	rm -f $(PROGRAM) *.o
