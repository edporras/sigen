# change as needed
CC	= clang
AR      = ar rv

# Library Name (does not include "main" symbol)
MODULE_NAME = sigen
LIB_NAME = lib$(MODULE_NAME).a

# -DENABLE_DUMP enables output to ostream
# -DNDEBUG disbales assertions
# -DTRACE internal debug
MODULE_DEFS = \
	-DENABLE_DUMP \
	-DNDEBUG

# if not defined by your project, set GLOBAL_[INCLUDE|LIB]_PATH
# variables to pint to where the target include and lib
# directories are to copy the files into.  Header files will be
# copied into a directory named sigen in your include path.
INCLUDE_DEST = $(GLOBAL_INCLUDE_PATH)/$(MODULE_NAME)
LIB_DEST = $(GLOBAL_LIB_PATH)

# -----------------------------------------------
# shouldn't have to mess with anything below here
#
HDRS    = \
	sigen.h \
	types.h \
        table.h \
	pat.h \
	pmt.h \
	cat.h \
        nit.h \
	sdt.h \
	eit.h \
	tdt.h \
	tot.h \
	bat.h \
	other_tables.h \
        descriptor.h \
	stream_desc.h \
        dvb_desc.h \
	nit_desc.h \
	linkage_desc.h \
	pmt_desc.h \
	sdt_desc.h \
	eit_desc.h \
	ssu_desc.h \
        tstream.h \
	packetizer.h \
	utc.h \
	dump.h \
	util.h \
        version.h \
	dvb_defs.h

SRCS    = \
        tstream.cc \
	packetizer.cc \
	utc.cc \
	dump.cc \
	util.cc \
	table.cc \
	descriptor.cc \
        pat.cc \
	pmt.cc \
	cat.cc \
	nit.cc \
	sdt.cc \
	eit.cc \
	tdt.cc \
	tot.cc \
	bat.cc \
	other_tables.cc \
        stream_desc.cc \
	dvb_desc.cc \
	nit_desc.cc \
	linkage_desc.cc \
	pmt_desc.cc \
	sdt_desc.cc \
	eit_desc.cc \
	ssu_desc.cc \
        version.cc

OBJS    = $(SRCS:.cc=.o)

# commands used here
#
CP      = cp -p -f
DEPEND  = makedepend -f.depend -m
DEPENDFLAGS = -I../
MKDIR	= -mkdir -p
RANLIB  = ranlib
RM      = rm -f
STRIP   = strip

# options
INCLUDE = -I../
CFLAGS	= -g -O2 -Wall -Wno-overloaded-virtual $(MODULE_DEFS)

# rules
all:    $(LIB_NAME)
	@echo "Done."

%.o : %.cc
	$(CC) $(INCLUDE) -c $(CFLAGS) $< -o $@

$(LIB_NAME): $(OBJS)
	$(RM) $(LIB_NAME)
	$(AR) $(LIB_NAME) $(OBJS)

install:: headers $(LIB_NAME)
	$(MKDIR) $(LIB_DEST)
	$(CP) $(LIB_NAME) $(LIB_DEST)

headers:
	$(MKDIR) $(INCLUDE_DEST)
	$(CP) $(HDRS) $(INCLUDE_DEST)

clean::
	$(RM) $(OBJS)
	$(RM) $(LIB_NAME)
	$(RM) core
	$(RM) .depend.bak

depend::
	$(DEPEND) -- $(DEPENDFLAGS) -- $(SRCS)

include .depend
# DO NOT DELETE
