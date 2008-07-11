export DHS_BASE=${GEMINI_TOP}/opt/dhs
export PATH=${PATH}:${DHS_BASE}/bin:${DHS_BASE}/scripts
export IMP_STARTUP=${DHS_BASE}/var/local-config/imp_startup
export IMP_SCRATCH=${DHS_BASE}/var/tmp
export DEFAULT_CONFIG_DIR=${DHS_BASE}/var/local-config/default_config_dir
export DHS_STAGING=${DHS_BASE}/var/local-config/staging

if [ ! -e $IMP_SCRATCH ]; then
	mkdir -p ${IMP_SCRATCH}
	chmod -R 777 ${IMP_SCRATCH}
fi
