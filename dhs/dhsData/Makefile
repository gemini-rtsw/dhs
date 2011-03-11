# Makefile

NAME = dhsData
SOURCES = arg.C cmd.C con.C config.C ctl.C dbm.C dtsDhs.C get.C list.C main.C msg.C put.C putFits.C putSds.C seq.C store.C
LIBS = -lsf -ldd -ldhs++ -ldhs -ldhsSta -ldhsGen -lpress -lgen -lad $(TCLTK_LDLIBS)
ifneq ($(SYBASE),)
      LIBS += -lfh -ldb -lcrc
endif

include ../conf/cxx_bin.mk
