# Include makefile for C libraries
# 
# Before including, define:
#
# NAME = base name of library
# SOURCES = list of C sources

include ../conf/include.mk

OBJECTS = $(SOURCES:.c=.o)
LIBRARY = lib$(NAME).a

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar r $(LIBRARY) $?
	$(RANLIB) $(LIBRARY)

install: 

clean:
	rm -f $(OBJECTS) $(LIBRARY)

etags:
	etags ${ETAGS_SOURCES}
