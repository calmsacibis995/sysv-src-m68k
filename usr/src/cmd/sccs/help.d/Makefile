#	@(#)Makefile	2.1	 -- SCCS helpfile installation

OL =
HELPLIB = $(OL)/usr/lib/help

FILES1 = ad bd cb cm cmds co de default
FILES2 = ge he prs rc un ut vc

helpfiles:
	-mkdir $(HELPLIB)
	cp $(FILES1) \
		$(FILES2) $(HELPLIB)
	-cd $(HELPLIB); chmod 664 $(FILES1) \
					$(FILES2)
	-@cd $(HELPLIB); chgrp bin $(FILES1) \
					$(FILES2) .
	-@cd $(HELPLIB); chown bin $(FILES1) \
					$(FILES2) .
