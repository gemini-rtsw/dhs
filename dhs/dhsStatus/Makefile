# Makefile

NAME = dhsStatus
SOURCES = arg.C config.C main.C msg.C staDhs.C staMonitor.C subscriber.C
ifneq ($(EPICS_BASE),)
      SOURCES += staChannel.C
      SOURCES += merger.C
endif


LIBS = -ldhs++ -ldhs -ldhsGen -ldhsSta -lgen $(EPICS_LDLIBS) $(TCLTK_LDLIBS)
ifneq ($(SYBASE),)
      LIBS += -ldb 
endif

include ../conf/cxx_bin.mk
