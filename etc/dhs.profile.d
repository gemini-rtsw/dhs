export DHS_BASE=${GEMINI_TOP}/opt/dhs
export PATH=${PATH}:${DHS_BASE}/bin:${DHS_BASE}/scripts
export IMP_STARTUP=${DHS_BASE}/var/local-config/imp_startup
export IMP_SCRATCH=${DHS_BASE}/var/tmp
export IMP_KEY=0xffff
export DEFAULT_CONFIG_DIR=${DHS_BASE}/var/local-config/default_config_dir
export DHS_STAGING=${DHS_BASE}/var/local-config/staging
DHS_SERVERS=$DHS_BASE/etc/server.conf.$GEMINI_SITE
hostname=`hostname -s`

if [ "`grep $hostname $DHS_SERVERS`" != "" ]; then
	export DHS_USER=gemdhs
else
	export DHS_USER=telops
fi

if [ ! -e $IMP_SCRATCH ]; then
	mkdir -p ${IMP_SCRATCH}
	chmod -R 777 ${IMP_SCRATCH}
fi
