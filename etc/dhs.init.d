#!/bin/bash
#
#       /gemsoft/etc/init.d/dhs
#
# Starts the Gemini dhs application. 
# It uses nasty techniques: su -c "GemBootServer server" dhsuser, for example.
# This is necessary, because eventually started client tools and the
# services need to have the same $UID on a local machine! If you want
# to run the dhsQlTool this now works only successfully as dhsuser.


. /etc/rc.d/init.d/functions
. /etc/profile

if [ "$DEFAULT_CONFIG_DIR" = "" ] ; then
	export DEFAULT_CONFIG_DIR=${DHS_BASE}/var/local-config/default_config_dir
fi
if [ "$DHS_STAGING" = "" ] ; then
	export DHS_STAGING=${DHS_BASE}/var/local-config/staging
fi
if [ "$DHS_USER" = "" ] ; then
	export DHS_USER=software
fi

start()
{
	echo "Starting dhs services (master version)"
	set -x
	if [ "$USER" != "$DHS_USER" ]; then
		su -c "GemBootStart server" $DHS_USER
	else
		GemBootStart server
	fi
	set +x
}

stop()
{
	echo "Stopping dhs services"
	set -x
	if [ "$USER" != "$DHS_USER" ]; then
		su -c "GemBootStart stop" $DHS_USER
	else
		GemBootStart stop
	fi
	set +x
}


restart()
{
	stop
	sleep 2
	start
}

reload()
{
	stop
	sleep 2
	start
}

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    reload)
        reload
        ;;
    restart)
        restart
        ;;
    status)
        status
        ;;
    *)
        echo $"Usage: $0 {start|stop|status|restart|reload}"
        exit 1
esac

