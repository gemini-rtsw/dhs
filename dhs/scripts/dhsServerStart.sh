#!/bin/bash

if ps -fea |grep master|grep -v "grep master" ; then
	dhsServerStop.sh
fi

logfile="dhs_`date +'%Y%m%d%H%M%S'`.log"

echo "Starting DHS services:"
/etc/init.d/imp_master start
/etc/init.d/dhs start > $GEMINI_TOP/var/log/dhs/$logfile
