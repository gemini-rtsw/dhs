# Include makefile for C programs
# 
# Before including, define:
#
# NAME = name of program
# SOURCES = list of C sources
# LIBS = the local libraries to link (not counting external or system libs)

include ../conf/include.mk

OBJECTS = $(SOURCES:.c=.o) version.o
PROGRAM = $(NAME)


all: $(PROGRAM)

$(PROGRAM): $(OBJECTS) ${LOCAL_LIBS}
	$(CC) -o $(PROGRAM) $(OBJECTS) $(LDFLAGS) $(LDLIBS)

install:
	cp $(PROGRAM) $(RELEASE_DIR)

clean:
	rm -f $(OBJECTS) $(PROGRAM)

etags:
	etags ${ETAGS_SOURCES}
