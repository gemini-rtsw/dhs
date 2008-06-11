# Makefile

NAME = dhsStatus
SOURCES = arg.C config.C main.C merger.C msg.C staChannel.C staDhs.C staMonitor.C subscriber.C
LIBS = -ldhs++ -ldhs -ldhsGen -ldhsSta -lgen $(EPICS_LDLIBS) $(TCLTK_LDLIBS)
ifneq ($(SYBASE),)
      LIBS += -ldb 
endif

include ../conf/cxx_bin.mk
