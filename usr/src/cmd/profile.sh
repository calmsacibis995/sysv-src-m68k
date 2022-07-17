# @(#)profile.sh	2.1	
#	@(#)profile.sh	1.4

trap "" 1 2 3
M68000=STACKCHECK
TZ=MST7
export LOGNAME M68000 TZ
umask 022
case "$0" in
-sh | -rsh)
	trap : 1 2 3
	cat /etc/motd
	trap "" 1 2 3
	if mail -e
	then
		echo "you have mail"
	fi
	if [ $LOGNAME != root ]
	then
		news -n
	fi
	;;
-su)
	:
	;;
esac
trap 1 2 3
