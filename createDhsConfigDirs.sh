#!/bin/bash

source /etc/profile

/sbin/ldconfig

# create dhsuser account
if [ "`cat /etc/passwd | grep dhsuser`" = "" ] ; then 
	useradd -u 2945 -G gemini -d /gemsoft/opt/dhs/ -M dhsuser
fi


#chmod  0666 $GEMINI_TOP/opt/dhs/var/sample-config/imp_startup/*
## for now until we have a group gemsoft where these files could belong to
#chmod  0666 $GEMINI_TOP/opt/dhs/var/sample-config/default_config_dir/*

if [ "$GEMINI_SITE" = "" ] ; then
	if [ "`/sbin/ifconfig | egrep 'addr:10\.'`" != "" ]; then
		export GEMINI_SITE=MK
	else
		export GEMINI_SITE=CP
	fi
fi
cd $GEMINI_TOP/opt/dhs/var
if [ ! -d local-config-$1 ]; then
	cp -a sample-config local-config-$1
fi
HOSTNAME=$(/bin/hostname -s)
if ([ -z $HOSTNAME ] || [ $HOSTNAME = localhost ]) && [ -e /root/postvars ] ; then
	HOSTNAME=$(sed -n "s/HOSTNAME:\([^.]*\).*/\1/p" < /root/postvars)
fi
[ -e local-config ] && rm local-config
if [ ! -d "$HOSTNAME" ]; then
	DHS_SERVERS=$GEMINI_TOP/opt/dhs/etc/server.conf.$GEMINI_SITE
	if [ -e $DHS_SERVERS ] && [ "`grep $HOSTNAME $DHS_SERVERS`" = "" ] && [ -d "ops-$GEMINI_SITE" ] ; then
		ln -sn ops-$GEMINI_SITE local-config &>/dev/null
	else
		ln -sn local-config-$1 local-config &>/dev/null
	fi
else
	ln -sn "$HOSTNAME" local-config &>/dev/null
fi

rm -rf auto-config
cp -a sample-config auto-config

chown -R dhsuser $GEMINI_TOP/opt/dhs/var
chgrp -R gemini $GEMINI_TOP/opt/dhs/var
chmod -R 775 $GEMINI_TOP/opt/dhs/var

## create auto configuration based on dhs.conf.$GEMINI_SITE

sed -e '/#.*/ d' -e '/^$/ d' < $GEMINI_TOP/tmp/dhs.conf.${GEMINI_SITE} > $GEMINI_TOP/tmp/dhs.conf.tmp
cd $GEMINI_TOP/opt/dhs/var/sample-config/default_config_dir
echo sed \\ > $GEMINI_TOP/tmp/sedscript.tmp
awk '{printf "-e \"s/%s/%s/g\" \\\n", $1,$2}' $GEMINI_TOP/tmp/dhs.conf.tmp >> $GEMINI_TOP/tmp/sedscript.tmp
chmod 755 $GEMINI_TOP/tmp/sedscript.tmp
for i in `ls`; do 
$GEMINI_TOP/tmp/sedscript.tmp < $i > $GEMINI_TOP/opt/dhs/var/auto-config/default_config_dir/$i
done
#cp $GEMINI_TOP/opt/dhs/var/auto-config/imp_startup/IMP_Startup.localhost $GEMINI_TOP/opt/dhs/var/auto-config/imp_startup/IMP_Startup.`/bin/hostname -s`
rm -f $GEMINI_TOP/tmp/sedscript.tmp 
rm -f $GEMINI_TOP/tmp/dhs.conf*
#for i in `ls`; do 
#sed \
#-e 's/STORE_HOST/dhsstorage/g' \
#-e 's/OLDP_HOST/dhsoldp/g' \
#-e 's/STA_HOST/dhsstatus/g' \
#-e 's/QLS_HOST/dhsqls/g' \
#-e 's/CMD_HOST/dhscmd/g' \
#-e 's/TOOL1_HOST/dhstool1/g' \
#-e 's/OCS_HOST/dhsocs/g' \
#-e 's/SIM_HOST/dhssad/g' \
#-e 's/DTS_HOST/dhsdtsremote/g' \
#< $i > ../../auto-config/default_config_dir/$i
#done

rm $0
