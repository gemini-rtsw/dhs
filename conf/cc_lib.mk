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

all: $(LIBRARY) $(SHARED_LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar r $(LIBRARY) $?
	$(RANLIB) $(LIBRARY)

$(SHARED_LIBRARY): $(OBJECTS)
	$(CC) $(SHLIB_LDFLAGS) $(CFLAGS) -o $(SHARED_LIBRARY) $(OBJECTS) ${LOCAL_LDFLAGS} $(SHARED_LIBS)

install: 
	cp $(LIBRARY) $(SHARED_LIBRARY) $(RELEASE_DIR)/lib

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(SHARED_LIBRARY)

etags:
	etags ${ETAGS_SOURCES}

# Automatic dependencies (from GNU make doc example)
%.d: %.c
	set -e; $(CC) -MM $(CFLAGS) $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; [ -s $@ ] || rm -f $@

-include $(SOURCES:.c=.d)
