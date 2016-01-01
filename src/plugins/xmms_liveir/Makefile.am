lib_LTLIBRARIES = libxmms_liveir.la

libxmms_liveir_la_METASOURCES = AUTO
libxmms_liveir_la_SOURCES = xmms_liveir.c about_liveir.c configure_liveir.c xmms_liveir.h

SOMAJOR = @libliveir_somajor@
SOMINOR = @libliveir_sominor@
SOSUBMINOR = @libliveir_subminor@
 
libxmms_liveir_la_CFLAGS  = `xmms-config --cflags` `gtk-config --cflags`
libxmms_liveir_la_LDFLAGS = `xmms-config --cflags --libs` `gtk-config --cflags --libs` -no-undefined -version-info $(SOMAJOR):$(SOMINOR):$(SOSUBMINOR)

libxmms_liveir_la_LIBADD = @livetools_libraries@

INCLUDES = @livetools_includes@

libdir = `xmms-config --general-plugin-dir`
