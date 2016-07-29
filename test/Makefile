LIBDIR	=
INCDIR	=

LIBDEPS	= $(LIBDIR)/libsigen.a
PROJLIBS =
LIBS	= -L$(LIBDIR) -lsigen

SYSDEFS	=

# project defines
PROJDEFS = -DENABLE_DUMP

CC      = g++
CDEFS	=
CCFLAGS	= -Wall -O2 -Wno-overloaded-virtual
#-g
DEBUGFLAGS =
LD	= $(CC)
LDINCS	= -I. -I$(INCDIR)

## ============================================
## shouldn't have to modify anything below here
ALLINCLUDES = -I$(INCDIR)
#-I/usr/include/g++-3
ALLDEFINES = $(SYSDEFS) $(CDEFS) $(PROJDEFS)

CFLAGS  = $(DEBUGFLAGS) $(CCFLAGS) $(ALLDEFINES) $(ALLINCLUDES)
LDFLAGS = $(ALLDEFINES) $(LDINCS) $(PTFLAGS)

# the project files
EXEC	= sigentest
SRCS	= main.cc
OBJS	= $(SRCS:.cc=.o)

DEPEND	= makedepend -f.depend
DEPENDFLAGS = $(ALLINCLUDES) $(ALLDEFINES) -w
RM 	= rm -f
STRIP	= strip

all:	$(EXEC)
	chmod a+x $(EXEC)

$(EXEC): $(OBJS) $(LIBDEPS)
	$(RM) $(EXEC)
	$(LD) $(LDFLAGS) -o $(EXEC) $(OBJS) $(PROJLIBS) $(LIBS)

%.o : %.cc
	$(CC) $(INCLUDE) -c $(CFLAGS) $< -o $@

clean::
	$(RM) $(OBJS)
	$(RM) $(EXEC)
	$(RM) core
	$(RM) .depend.bak

strip:
	$(STRIP) $(EXEC)

depend::
	$(DEPEND) $(DEPENDFLAGS) --  -- $(SRCS)

