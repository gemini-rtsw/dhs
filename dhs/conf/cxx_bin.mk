# Include makefile for C++ programs
# 
# Before including, define:
#
# NAME = name of program
# SOURCES = list of C++ sources
# LIBS = the local libraries to link (not counting external or system libs)

include ../conf/include.mk

OBJECTS = $(SOURCES:.C=.o) version.o
PROGRAM = $(NAME)
CXX = g++ -m32

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS) ${LOCAL_LIBS}
	$(CXX) -o $(PROGRAM) $(OBJECTS) $(LDFLAGS) $(LDLIBS)

install: $(INSTALL_TARGET)
	cp $(PROGRAM) $(RELEASE_DIR)/bin

clean:
	rm -f $(OBJECTS) $(PROGRAM) $(SOURCES:.C=.d)

etags:
	etags ${ETAGS_SOURCES}

# Automatic dependencies (from GNU make doc example)
%.d: %.C
	set -e; $(CXX) -MM $(CXXFLAGS) $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; [ -s $@ ] || rm -f $@

ifeq (, $(findstring "clean", "$(MAKECMDGOALS)"))
-include $(SOURCES:.C=.d)
endif
