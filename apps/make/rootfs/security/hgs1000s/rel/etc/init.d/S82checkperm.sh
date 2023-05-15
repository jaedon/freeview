#!/bin/sh

echo "* Check the configuration of the file permission"

if [ -f /var/lib/humaxtv/browser/cookies4.dat ]
then
	OWNERSHIP=`ls -ld /var/lib/humaxtv/browser/cookies4.dat | awk '{print $3}'`
else
	OWNERSHIP=
fi

if [ ! -f /var/lib/humaxtv/.perm ] || [[ "$OWNERSHIP" == "root" ]]
then
	echo "** Changing the permission of files"
	if [ -f /var/lib/humaxtv/.umount-proc-info ]
	then
		chown obama:settop /var/lib/humaxtv/.umount-proc-info
	fi
	if [ -f /var/lib/humaxtv/TZ ]
	then
		chown obama:settop /var/lib/humaxtv/TZ
	fi
	if [ -d /var/lib/humaxtv/browser ]
	then
		chown corsair:settop -R /var/lib/humaxtv/browser
	fi
	if [ -d /var/lib/humaxtv/cas ]
	then
		chown obama:settop -R /var/lib/humaxtv/cas
	fi
	if [ -d /var/lib/humaxtv/dlogger ]
	then
		chown dama:settop -R /var/lib/humaxtv/dlogger
	fi
	if [ -d /var/lib/humaxtv_backup/dlogger ]
	then
		chown dama:settop -R /var/lib/humaxtv_backup/dlogger;
	fi
	if [ -f /var/lib/humaxtv/dtcp.rng ]
	then
		chown imma:settop /var/lib/humaxtv/dtcp.rng
	fi
	if [ -f /var/lib/humaxtv/resolv.conf ]
	then
		chown nima:settop /var/lib/humaxtv/resolv.conf
	fi
	if [ -f /var/lib/humaxtv/registry.reg ]
	then
		chown hama:settop /var/lib/humaxtv/registry.reg
	fi
	if [ -f /var/lib/humaxtv_backup/registry.reg ]
	then
		chown hama:settop /var/lib/humaxtv_backup/registry.reg
	fi
        if [ -d /var/lib/humaxtv_user/ipepg ]
        then
                chown dama:settop -R /var/lib/humaxtv_user/ipepg;
        fi
        if [ -d /var/lib/humaxtv_user/tvtvepg ]
        then
                chown dama:settop -R /var/lib/humaxtv_user/tvtvepg;
        fi
	echo "check drm permisstion "
	if [ -d /var/lib/humaxtv/drm ]
	then
		chown settop:settop -R /var/lib/humaxtv/drm;
		chmod 770 -R /var/lib/humaxtv/drm;
	fi
	echo 1 > /var/lib/humaxtv/.perm
else
	echo "** The permission of files was already changed"
fi
