MAIN=cmetrics
SOURCE=cmetrics.py

BINdir=${prefix}/bin
dist_BIN_SCRIPTS = $(MAIN)

SUBDIRS = mccabe halstead kdsi utils 

EXTRA_DIST = AUTHORS COPYING INSTALL README TODO $(SOURCE)
AUTOMAKE_OPTIONS = foreign
CLEANFILES = $(MAIN)

cmetrics: $(SOURCE)
	@echo "#!$(PYTHON)" > $(MAIN)
	@cat $(SOURCE) >> $(MAIN)
	@chmod a+x $(MAIN)