# -*-makefile-*-
# Common make definitions

# Root DHS source dir
TOP = ..

# Release install directory
RELEASE_DIR = $(TOP)/release

# System dependent libraries
SYSLIBS = -lpthread -lrt  -lnsl -lm


# DRAMA Directories
DRAMA = /home/abrighto/work/gemini/drama-v1.4.2/release
DRAMA_OS = linux_x86

DRAMA_CONFIG = /home/abrighto/work/gemini/drama-v1.4.2/release/config/r3_1_3
DRAMA_STATUS = /home/abrighto/work/gemini/drama-v1.4.2/release/status/r0_3_3
IMP_DIR = /home/abrighto/work/gemini/drama-v1.4.2/release/imp/r1_6_1
SDS_DIR = /home/abrighto/work/gemini/drama-v1.4.2/release/sds/r1_4_13
ERS_DIR = /home/abrighto/work/gemini/drama-v1.4.2/release/ers/r1_3_1

DRAMA_INCLUDES = \
	-I${IMP_DIR}  \
	-I${IMP_DIR}/${DRAMA_OS} \
	-I${SDS_DIR} \
	-I${ERS_DIR} \
	-I${DRAMA_CONFIG} \
	-I${DRAMA_STATUS}

DRAMA_LDLIBS = -L${IMP_DIR}/${DRAMA_OS} -limp -L${SDS_DIR}/${DRAMA_OS} -lsds -L${ERS_DIR}/${DRAMA_OS} -lers


# Root dir for Sybase include files and libs
SYBASE = /opt/sybase-12.5/OCS-12_5
SYBASE_INCLUDES = -I$(SYBASE)/include
SYBASE_LDLIBS = -L$(SYBASE)/lib -lsybdb

# Base dir for Epics
EPICS = $(TOP)/../epics/base
EPICS_INCLUDES = -I$(EPICS)/include
EPICS_LDLIBS = -L$(EPICS)/lib/${HOST_ARCH} -lca -lCom


# CFITSIO (relative to the source dirs)
CFITSIO = $(TOP)/../cfitsio
CFITSIO_INCLUDES = -I$(CFITSIO)
CFITSIO_LDLIBS = -L$(CFITSIO) -lcfitsio

# C include options
INCLUDES = -I. \
	-I$(TOP)/ad \
	-I$(TOP)/am \
	-I$(TOP)/cd \
	-I$(TOP)/db \
	-I$(TOP)/dd \
	-I$(TOP)/dhs \
	-I$(TOP)/dvd \
	-I$(TOP)/gen \
	-I$(TOP)/mfs \
	-I$(TOP)/press \
	-I$(TOP)/sf \
	-I$(TOP)/tp \
	${DRAMA_INCLUDES} \
	${SYBASE_INCLUDES} \
	${EPICS_INCLUDES} \
	${CFITSIO_INCLUDES}

# C++ include options
CXX_INCLUDES = $(INCLUDES) \
	-I$(TOP)/arrayMath \
	-I$(TOP)/crc \
	-I$(TOP)/dhsGen \
	-I$(TOP)/dhs++ \
	-I$(TOP)/dhsSta \
	-I$(TOP)/fh \
	-I$(TOP)/gen++ \
	-I$(TOP)/mfs \
	-I$(TOP)/qldp \
	-I$(TOP)/fitsio

# Define options
DEFINES =  -DHAVE_DIRENT_H=1 -DSTDC_HEADERS=1 -DHAVE_FCNTL_H=1 -DHAVE_LIMITS_H=1 -DHAVE_UNISTD_H=1 -DHAVE_MNTENT_H=1 -DHAVE_SYS_STATVFS_H=1 -DSIZEOF_LONG=4 -DRETSIGTYPE=void -DHAVE_VPRINTF=1 -DHAVE_MKDIR=1 -DHAVE_RMDIR=1 -DHAVE_STRERROR=1 -DHAVE_STRSTR=1 -DHAVE_STRTOD=1  -DDSTDARG_OK -D_POSIX -DPOSIX_SOURCE

# C Compiler options
CFLAGS = -g ${DEFINES} ${INCLUDES} ${CXX_INCLUDES} 

# C++ Compiler options
CXXFLAGS = $(CFLAGS)

# Linker options
LDFLAGS = -g 

# local library dependencies
LOCAL_LIBS = \
	$(TOP)/arrayMath/libarrayMath.a \
	$(TOP)/dhsGen/libdhsGen.a \
	$(TOP)/dhs++/libdhs++.a \
	$(TOP)/dhsSta/libdhsSta.a \
	$(TOP)/fh/libfh.a \
	$(TOP)/gen++/libgen++.a \
	$(TOP)/mfs/libmfs.a \
	$(TOP)/qldp/libqldp.a \
	$(TOP)/fitsio/libfitsio.a \
	$(TOP)/ad/libad.a \
	$(TOP)/am/libam.a \
	$(TOP)/cd/libcd.a \
	$(TOP)/db/libdb.a \
	$(TOP)/dd/libdd.a \
	$(TOP)/dhs/libdhs.a \
	$(TOP)/dvd/libdvd.a \
	$(TOP)/gen/libgen.a \
	$(TOP)/mfs/libmfs.a \
	$(TOP)/press/libpress.a \
	$(TOP)/sf/libsf.a \
	$(TOP)/tp/libtp.a \
	$(TOP)/crc/libcrc.a \

# LD flags for local libs
LOCAL_LDFLAGS = \
	-L$(TOP)/arrayMath \
	-L$(TOP)/dhsGen \
	-L$(TOP)/dhs++ \
	-L$(TOP)/dhsSta \
	-L$(TOP)/fh \
	-L$(TOP)/gen++ \
	-L$(TOP)/mfs \
	-L$(TOP)/qldp \
	-L$(TOP)/fitsio \
	-L$(TOP)/ad \
	-L$(TOP)/am \
	-L$(TOP)/cd \
	-L$(TOP)/db \
	-L$(TOP)/dd \
	-L$(TOP)/dhs \
	-L$(TOP)/dvd \
	-L$(TOP)/gen \
	-L$(TOP)/mfs \
	-L$(TOP)/press \
	-L$(TOP)/sf \
	-L$(TOP)/tp \
	-L$(TOP)/crc \


# Libraries to link
LDLIBS = ${LOCAL_LDFLAGS} ${LIBS} ${CFITSIO_LDLIBS} ${DRAMA_LDLIBS} ${SYBASE_LDLIBS} $(SYSLIBS) 

# Set to ":" if there is no ranlib command
RANLIB = ranlib

# C compiler
CC = gcc

# C++ compiler
CXX = c++

# Script used to install files
INSTALL = /usr/bin/install -c

# Development source dirs
ETAGS_SOURCES = $(TOP)/*/*.[Cch]
#${DRAMA}/../drama_source/*/*.c ${DRAMA}/../drama_source/*/*.h
