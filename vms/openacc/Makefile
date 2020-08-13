TOPDIR=..

default: run

include ../Makefile.common

run: $(PROGRAM_)
	./$(PROGRAM_) $(ARGS)

PGCXX = pgc++
CFLAGS = -fast -Minfo=accel -ta=tesla

$(PROGRAM_): $(OBJFILES)
	        $(PGCXX) $(CFLAGS) $(CFLAGS_) $^ -o $@ $(LDFLAGS_)

%.o: $(SRCDIR)/%.cpp $(DEPS)
	        $(PGCXX) $(CFLAGS) $(CFLAGS_) -c -o $@ $<


