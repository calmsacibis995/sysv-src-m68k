#	@(#)checkall.sh	2.1	
# Warning: this is only a sample.
fsck $* /dev/dk10
dfsck -T/tmp/1 /dev/rdk11 - /dev/rdk[12345]
