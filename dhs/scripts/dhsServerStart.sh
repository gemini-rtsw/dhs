#!/bin/bash

if ps -fea |grep master|grep -v "grep master" ; then
	dhsServerStop.sh
fi

echo "Starting DHS services:"
/etc/init.d/imp_master start
/etc/init.d/dhs start
