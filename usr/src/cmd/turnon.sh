#	@(#)turnon.sh	2.1	
# Enter new games with mode 100 (544 for shell scripts).
for i in /usr/games/*
do
	if [ -f "$i" -a -x "$i" ]
	then
		chmod go+x "$i"
	fi
done
