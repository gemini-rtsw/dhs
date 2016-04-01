# Include makefile for C libraries
# 
# Before including, define:
#
# NAME = base name of library
# SOURCES = list of C sources

include ../conf/include.mk

OBJECTS = $(SOURCES:.c=.o)
LIBRARY = lib$(NAME).a
SHARED_LIBRARY = lib$(NAME).so
CC = gcc -m32
CXX = g++ -m32

all: $(LIBRARY) $(SHARED_LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar r $(LIBRARY) $?
	$(RANLIB) $(LIBRARY)

$(SHARED_LIBRARY): $(OBJECTS)
	$(CC) $(SHLIB_LDFLAGS) $(CFLAGS) -o $(SHARED_LIBRARY) $(OBJECTS) ${LOCAL_LDFLAGS} $(SHARED_LIBS)

install: $(INSTALL_TARGET)
	cp $(LIBRARY) $(SHARED_LIBRARY) $(RELEASE_DIR)/lib

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(SHARED_LIBRARY) $(SOURCES:.c=.d)

etags:
	etags ${ETAGS_SOURCES}

# Automatic dependencies (from GNU make doc example)
%.d: %.c
	set -e; $(CC) -MM $(CFLAGS) $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; [ -s $@ ] || rm -f $@

ifeq (, $(findstring "clean", "$(MAKECMDGOALS)"))
-include $(SOURCES:.c=.d)
endif
