# Makefile

# make subdirectories
SUBDIRS = \
	gen \
	db \
	ad \
	mfs \
	crc \
	cd \
	dvd \
	tp \
	press \
	am \
	gen++ \
	dhsGen \
	dhs++ \
	dhsSta \
	dd \
	sf \
	dhs \
	fh \
	arrayMath \
	qldp \
	fitsio \
	dhsCommand \
	dhsConsole \
	dhsData \
	dhsGet \
	dhsHistory \
	dhsPut \
	dhsQlServer \
	dhsQlTool \
	dhsStatus \
	dhsStorage \
	dhs++Client \
	dhs++Server \
	dhsClient \
	dhsCmdSender \
	dhsInstSim \
	dhsServer \
	dhsSim4Data \
	dhsTestCmdr \
	fits2sds \
	cdIngest \
	cdOnline \
	mediaCleanup \
	mediaPrep \
	mediaQueue \
	mediaWrite \
	testCmdr \
	tpIngest \



SCRIPTS = \
	scripts/dhsCleanup \
	scripts/GemBootStart \
	scripts/MakeClassicalMedia \
	scripts/MakePrImages \
	scripts/MakeUserMedia \
	scripts/newsyslog

RELEASE_DIRS = \
	release \
	release/bin \
	release/include \
	release/lib \
	release/scripts \
	release/sql \
	release/config \
	release/doc \
	release/man \
	release/man/man1 \
	release/man/man3 \
	release/man/mann


all clean: FORCE
	for i in $(SUBDIRS); do (cd $$i; echo "$$i:"; $(MAKE) $@) done

distclean: clean
	rm -f config.status config.cache config.log confdefs.h include.mk
	rm -rf release */*.d

install:
	for i in ${RELEASE_DIRS}; do test -d $$i || mkdir $$i; done
	cp */*.h release/include
	cp ${SCRIPTS} release/scripts
	cp */*.config release/config
	cp */html/* release/doc
	cp */*.sql release/sql
	cp */*.1 release/man/man1
	cp */*.3 release/man/man3
	cp */*.n release/man/mann
	for i in $(SUBDIRS); do (cd $$i; echo "$$i:"; $(MAKE) $@) done

FORCE:
