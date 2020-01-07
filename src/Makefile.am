# what flags you want to pass to the C compiler & linker
AM_CXXFLAGS = $(SIGEN_BUILD_CPPFLAGS)

#AM_LDFLAGS =

#sigen_LDADD =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libsigen.la
libsigen_la_SOURCES = \
	cat.cc \
	descriptor.cc \
	dvb_desc.cc \
	eacem_desc.cc \
	eit.cc \
	eit_desc.cc \
	language_code.cc \
	linkage_desc.cc \
	mpeg_desc.cc \
	nit_bat.cc \
	nit_desc.cc \
	other_tables.cc \
	packetizer.cc \
	pat.cc \
	pmt.cc \
	pmt_desc.cc \
	sdt.cc \
	sdt_desc.cc \
	ssu_desc.cc \
	table.cc \
	tdt.cc \
	tot.cc \
	tstream.cc \
	utc.cc \
	version.cc

libsigenincludedir = $(includedir)/sigen
libsigeninclude_HEADERS = \
	cat.h \
	descriptor.h \
	dump.h \
	dvb_defs.h \
	dvb_desc.h \
	eacem_desc.h \
	eit.h \
	eit_desc.h \
	language_code.h \
	linkage_desc.h \
	mpeg_desc.h \
	nit_bat.h \
	nit_desc.h \
	other_tables.h \
	packetizer.h \
	pat.h \
	pmt.h \
	pmt_desc.h \
	sdt.h \
	sdt_desc.h \
	sigen.h \
	ssu_desc.h \
	table.h \
	tdt.h \
	tot.h \
	tstream.h \
	types.h \
	utc.h \
	version.h


if ENABLE_DUMP_SRC
libsigen_la_SOURCES += dump.cc
endif

distclean-local:
	-rm -f Makefile.in
