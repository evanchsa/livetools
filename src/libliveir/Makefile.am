
lib_LTLIBRARIES = libliveir.la
libliveir_la_SOURCES = liveir.c alsa_device.c	\
			device.c decoder.c 	\
			remotes.c livetools.h
include_HEADERS = livetools.h

SOMAJOR = @libliveir_somajor@
SOMINOR = @libliveir_sominor@ 
SOSUBMINOR = @libliveir_subminor@

libliveir_la_LDFLAGS = -no-undefined -version-info $(SOMAJOR):$(SOMINOR):$(SOSUBMINOR)

AM_CFLAGS = -Wall
