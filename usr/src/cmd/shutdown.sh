#	@(#)shutdown.sh	2.1.1.1	

sync; sync; sync
USAGE="Usage: $0 [ seconds ]"	# seconds in range 0-999
if [ $# -eq 0 ]
then
	GRACE=60
elif [ $# -eq 1 ]
then
	case $1 in
		[0-9] | \
		[0-9][0-9] | \
		[0-9][0-9][0-9] ) GRACE=$1;;

		* ) echo $USAGE 1>&2
			exit 1;;
	esac
else
	echo $USAGE 1>&2
	exit 1
fi

a=`pwd`
if [ "$a" != "/" -a "$a" != "/etc" ]
then
	echo "$0: You must be in the root directory (/) to use $0" 1>&2
	exit 1
fi
echo "\nSHUTDOWN PROGRAM\n"
date

T=/tmp
trap "rm -f $T/$$; exit 0" 0 1 2 3 15

cd /
a="`who | wc -l`"
if [ $a -gt 1 ]
then
	echo "\nDo you want to send your own message? (y or n):   \c"
	read b
	if [ "$b" = "y" ]
	then
		echo "\nType your message followed by ctrl-d ...\n"
		su adm -c /etc/wall
	else
		su adm -c /etc/wall <<!

PLEASE LOG OFF NOW ! ! !
System maintenance about to begin.
All processes will be killed in $GRACE seconds.
!
	fi
	sleep $GRACE
fi
/etc/wall <<!

SYSTEM BEING BROUGHT DOWN NOW ! ! !
!
if [ $GRACE -ne 0 ]
then
	sleep 10
fi

echo "\nBusy out (push down) the appropriate phone lines for this system."
echo "\nDo you want to continue? (y or n):   \c";
read b
if [ "$b" = "y" ]
then
	/usr/lib/lpshut
	echo "\nLine printer scheduler stopped."
	/usr/lib/acct/shutacct
	echo "Process accounting stopped."
	/etc/errstop
	echo "Error logging stopped."

	echo "\nAll currently running processes will now be killed."
	/etc/killall

    mount | sed -n -e '/^\/ /d' -e 's/^.* on\(.*\) read.*/umount \1/p' | sh -
	/etc/init s;
	echo "Wait for 'INIT: SINGLE USER MODE' before halting."
	sync;sync;sync;
    mount | sed -n -e '/^\/ /d' -e 's/^.* on\(.*\) read.*/umount \1/p' | sh -
	sync;sync;sync;
else
	echo "For help, call your system administrator."
fi
