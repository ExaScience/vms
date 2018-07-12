
CXXFLAGS=-g -O0 -fsanitize=address 
CXXFLAGS+=-DDT_DOUBLE

LDFLAGS=-lsystemc

FILES=main.cpp predict.cpp
BINARY=native

-include Makefile.deps

$(BINARY): Makefile.deps $(FILES)
	$(CXX) $(CXXFLAGS) $(FILES) -o $(BINARY) $(LDFLAGS)

Makefile.deps: $(FILES) $(HEADERS)
	makedepend -- $(CFLAGS) -- $(FILES) -f- > Makefile.deps
