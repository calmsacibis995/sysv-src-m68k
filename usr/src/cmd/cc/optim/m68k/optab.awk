#	@(#)optab.awk	2.1	
#	@(#) optab.awk:	2.1 83/07/08
BEGIN		{ OFS = ""; FS = "\t"; ORS = ""; first = 1 }
$1 == "/*"	{ print; print "\n"; next }
first == 0	{ print "," }
first != 0	{ print "{"; comment = "" }
length(comment) != 0 \
		{ print "\t/* ", comment, " */"; comment = "" }
		{ print "\n" }
		{ print "    { \"", $1, "\",\t" }
		{ print $2, ",\t" }
		{ print $3, ",\t", $4, ",\t{", $5, "},\t", $6, ",\t" $7, \
			",\t", $8, " }" }
		{ first = 0 }
NF > 8		{ comment = $9 }
END		{ print "\n};\n" }
