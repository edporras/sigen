check_PROGRAMS = dvb_builder
dvb_builder_CFLAGS = @CHECK_CFLAGS@
dvb_builder_LDADD = $(top_builddir)/src/libsigen.la

dvb_builder_SOURCES = \
	dvb_builder.cc \
	pat_test.cc \
	pmt_test.cc \
	nit_test.cc \
	sdt_test.cc \
	bat_test.cc \
	eit_test.cc \
	tot_test.cc \
	cat_test.cc \
	tdt_test.cc \
	rst_test.cc \
	st_test.cc \
	eacem_test.cc \
	$(top_builddir)/src/sigen.h


TESTS = \
	test_bat.sh \
	test_cat.sh \
	test_eit.sh \
	test_nit.sh \
	test_pat.sh \
	test_pmt.sh \
	test_rst.sh \
	test_sdt.sh \
	test_st.sh \
	test_tdt.sh \
	test_tot.sh \
	test_eacem.sh

distclean-local:
	-rm -f Makefile.in
