check_PROGRAMS = test_table_gen
test_table_gen_SOURCES = test_table_gen.cc $(top_builddir)/src/sigen.h
test_table_gen_CFLAGS = @CHECK_CFLAGS@
test_table_gen_LDADD = $(top_builddir)/src/libsigen.la

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
