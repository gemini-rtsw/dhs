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

DHS_SERVERS=$DHS_BASE/etc/server.conf.$GEMINI_SITE 

start()
{
	hostname=`hostname -s`
	if [ `grep $hostname $DHS_SERVERS` != "" ]; then
		echo "Starting dhs services (master version)"
		set -x
		su -c "GemBootStart server" $DHS_USER
		set +x
	else
		echo "Starting dhs services (slave version)"
		set -x
		su -c "GemBootStart start" $DHS_USER
		set +x
	fi
}

stop()
{
	echo "Stopping dhs services"
	set -x
	su -c "GemBootStart stop" $DHS_USER
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

