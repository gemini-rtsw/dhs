# Include makefile for C++ libraries
# 
# Before including, define:
#
# NAME = base name of library
# SOURCES = list of C++ sources

include ../conf/include.mk

OBJECTS = $(SOURCES:.C=.o)
LIBRARY = lib$(NAME).a

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar r $(LIBRARY) $?
	$(RANLIB) $(LIBRARY)

install: 
	cp $(LIBRARY) $(RELEASE_DIR)/lib

clean:
	rm -f $(OBJECTS) $(LIBRARY)

etags:
	etags ${ETAGS_SOURCES}

# Automatic dependencies (from GNU make doc example)
%.d: %.C
	set -e; $(CXX) -MM $(CXXFLAGS) $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; [ -s $@ ] || rm -f $@

-include $(SOURCES:.C=.d)
