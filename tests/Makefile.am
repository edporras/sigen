check_PROGRAMS = dvb_builder
dvb_builder_SOURCES = dvb_builder.cc $(top_builddir)/src/sigen.h
dvb_builder_CFLAGS = @CHECK_CFLAGS@
dvb_builder_LDADD = $(top_builddir)/src/libsigen.la

TESTS = \
	test_bat.sh \
	test_cat.sh \
	test_eit.sh \
	test_nit.sh \
	test_pat.sh \
	test_pmt.sh \
	test_sdt.sh \
	test_tdt.sh \
	test_tot.sh

distclean-local:
	-rm -f Makefile.in
