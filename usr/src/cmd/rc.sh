#	@(#)rc.sh	2.1.1.1	

TZ=MST7
export TZ

if [ ! -f /etc/mnttab ]
then
	> /etc/mnttab
	/etc/devnm / | grep -v swap | grep -v root | /etc/setmnt
fi

set `who -r`
if [ $7 = 2 ]
then
# Mount user packs.
#	mount /dev/dk10 /usr	# mount usr file system
#	echo "Mounted /dev/dk10 on /usr."
# Set up process accounting
	rm -f /usr/adm/acct/nite/lock*
	/bin/su - adm -c /usr/lib/acct/startup
	echo "Process accounting started."
# Invoke ex/vi's session preservation notification
	/usr/lib/ex3.7preserve -
	echo "Expreserve notification performed."
# Set up error logging
	/usr/lib/errdemon
	echo "Error logging started."
# Set up system activity reporting
	#/bin/su - sys -c "/usr/lib/sa/sadc /usr/adm/sa/sa`date +%d` &"
	#mv /usr/adm/sulog /usr/adm/OLDsulog
	#echo "System activity reporting started."
# Set up clock daemon
	mv /usr/adm/cronlog /usr/adm/OLDcronlog
	> /usr/adm/cronlog
	/etc/cron
	echo "Cron started."
# Cleanup
	rm -f /tmp/*
	rm -f /usr/tmp/*
	#rm -f /usr/spool/lpd/*
	rm -f /usr/spool/uucp/LCK*
# Start line printer scheduler/daemon (choose one)
	rm -f /usr/spool/lp/SCHEDLOCK
	/usr/lib/lpsched
	echo "Line printer scheduler started."
	#rm -f /usr/spool/lpd/lock
	#rm -f /usr/spool/vpd/lock
	#/usr/lib/lpd
	#echo "Line printer daemon started."
# Go for it!
	echo "\n********** SYSTEM MULTI-USER `date` **********\n"
fi
