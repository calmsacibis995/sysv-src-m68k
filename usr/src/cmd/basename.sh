#	@(#)basename.sh	2.1	
a=`expr //${1-.} : '.*/\(.*\)'`
expr $a : "\(.*\)$2$" \| $a