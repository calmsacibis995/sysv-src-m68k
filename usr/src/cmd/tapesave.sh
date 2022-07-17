#	@(#)tapesave.sh	2.1	
# 'Volcopy backup to tape:'
#
#  The following script is to be used for full system backups
#  to tape.  It assumes that file systems to be saved are listed
#  in the /etc/rc file and that disk pack volume labels match
#  file system names.
#

FF_LOGDIR="/m1/cato/ff.log"
MORE="y"
USAGE="USAGE: ${0} [${VALID_FS}] [ /dev/rmt1 /dev/rmt11 ]"

trap 'umount /dev/rp1;umount /dev/rp6;exit 1' 1 2 3 14 15
trap 'umount /dev/rp1;umount /dev/rp6;exit 0' 0

# Mount all pertinent devices.

mount | grep "usr" > /tmp/$$.usrmnt
if [ -s /tmp/$$.usrmnt ]
then
	umount /dev/rp1;
else
	rm -fr /usr/tmp;
fi
rm -f /tmp/$$.usrmnt
mount /dev/rp1 /usr
mount /dev/rp6 /m1

VALID_FS=" root "`grep 'mount /dev/rp' /etc/rc | cut -d'/' -f4 | tr '\012' ' '`

ARG_CNT=$#
while [ "${MORE}" = "y" ]
do
	case ${ARG_CNT} in
	0)	DISK_DRIVE="/dev/r"
		while [ "${DISK_DRIVE}" = "/dev/r" ]
		do
			echo "Enter file system name (${VALID_FS}):"
			read FS_NAME
			if [ "${FS_NAME}" = "" ]
			then
				continue
			fi
			if [ "${FS_NAME}" = "root" ]
			then
				DISK_DRIVE="/dev/rrp0"
			else
				DISK_DRIVE="${DISK_DRIVE}"`grep 'mount /dev/rp' /etc/rc | grep "${FS_NAME}" | cut -d'/' -f3`
			fi
		done
		TAPE_DRIVE="invalid"
		while [ "${TAPE_DRIVE}" != "/dev/rmt1" -a \
			"${TAPE_DRIVE}" != "/dev/rmt11" ]
		do
			echo 'Enter tape drive (/dev/rmt1 /dev/rmt11):'
			read TAPE_DRIVE
		done
		;;
	2)	FS_NAME=${1}
		TAPE_DRIVE=${2}
		ARG_CNT=0
		if [ "${FS_NAME}" = "root" ]
		then
			DISK_DRIVE="/dev/rrp0"
		else
			DISK_DRIVE="/dev/r`grep 'mount /dev/rp' /etc/rc | grep ${FS_NAME} | cut -d'/' -f3`"
		fi
		if [ "${TAPE_DRIVE}" != "/dev/rmt1" -a \
		      ${TAPE_DRIVE} != "/dev/rmt11" -o \
		      ${DISK_DRIVE} = "/dev/r" ]
		then
			echo ${USAGE} >&2
			exit 1
		fi
		;;
	*)	echo ${USAGE} >&2
		exit 1
		;;
	esac
	
	PACK_VOLUME=${FS_NAME}
	TAPE_VOLUME=${FS_NAME}
	
	# The following command is required only as long as the
	# system has only two disk drives.  Once additional
	# drives are added, this command should be removed.
	# Also, it is important to periodically remove some
	# of the log files as they become obsolete since they
	# consume a great deal of space.
	
	if [ "$PACK_VOLUME" = "root" ]
	then
		PREFIX="";
		/etc/ff -s -u -p "${PREFIX}" ${DISK_DRIVE} \
			> ${FF_LOGDIR}/${FS_NAME}.`date +%m%d%y` 2>&1 &
	else
		PREFIX="/${PACK_VOLUME}";
	fi
	
	# If the tape is not labeled, then label as a new tape.

	/etc/labelit ${TAPE_DRIVE}
	if [ $? -gt 0 ]
	then
		/etc/labelit ${TAPE_DRIVE} ${FS_NAME} ${TAPE_VOLUME} -n
		if [ $? -gt 0 ]
		then
			exit 1
		fi
	fi

	# Copy the entire contents of the volume onto the tape(s).

	/etc/volcopy	-bpi1600 -feet2400 \
			${FS_NAME} \
			${DISK_DRIVE} ${PACK_VOLUME} \
			${TAPE_DRIVE} ${TAPE_VOLUME}

	# Maintain list of saved file systems.

	if [ $? -eq 0 ]
	then
		if [ "${FS_LIST}" = "" ]
		then
			FS_LIST="${FS_NAME}"
		else
			FS_LIST="${FS_LIST}, ${FS_NAME}"
		fi
	fi

	# Request whether additional file system backups
	# are desired.

	MORE="invalid"
	while [ "${MORE}" != "y" -a "${MORE}" != "n" ]
	do
		echo "More? (y or n)"
		read MORE
	done
done

if [ "${FS_LIST}" != "" ]
then
	echo "$0: File system(s): ${FS_LIST} successfully backed up."
else
	echo "$0: No file systems were successfully backed up."
fi
exit 0
