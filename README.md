C++ Si Generation Library
=========================

A C++ [MPG PSI](https://en.wikipedia.org/wiki/Program-specific_information) and
[DVB SI](https://en.wikipedia.org/wiki/Digital_Video_Broadcasting)
table generation library. The library has support most (if not all) tables: CAT,
PAT, PMT, NIT, SDT, BAT, TDT, TOT and their descriptors.


I wrote the initial version of this library
ca. 1997 and updated it around 2003 with a few minor tweaks done since
then. Nobody was maintaining it I was given permission to the source
so I could make changes to it if needed.



Building
========

(Apologies for the fugly Makefile but this is all we needed back then.)

Edit the Makefile and define the following to enable / disable the
feature:

  `-DENABLE_DUMP`: forces inclusion of methods and functions for
  outputing table / descriptor dump data to a c++ ostream.  All table
  / descriptor classes are defined with a `dump(ostream&)` method for
  this purpose.  For debugging, etc. to stdout, use the `DUMP()` macro
  defined in dump.h which is included only when this feature is
  enabled (calls to dump data are not compiled otherwise).

  Of course, you can choose to dump to a file by passing an `ofstream`
  as the argument to the dump methods.

  `-DTRACE`: internal debug macro.  You shouldn't need to use this
  unless you're curious or, worse yet, bored. :)

Then build:

```
  cd sigen
  make
```

To install into global include / lib paths, you should define the
variables `GLOBAL_INCLUDE_PATH` and `GLOBAL_LIB_PATH`.  For example,
under bash:

```
   export GLOBAL_INCLUDE_PATH="~/include"
   export GLOBAL_LIB_PATH="~/lib"
```

Then, install with the command:

```
  make install
```

This will take care of creating a directory named `sigen` in the
include path and copying the necessary headers into it.  Then it will
copy `libsigen.a` into the lib path.  You should use these global ones
to compile and link against instead of the library's work source
directory.


Sample Usage
============

See the file test/main.cc for a more thorough example. Here's a snippet:

```c++

    // some dummy data
    const ui16 network_pid = 0x20;
    const ui16 xport_stream_id = 0x10;
    const int num_services = 10;

    // the binary table data will be written to a TStream
    TStream t;


    //
    // Create an MPG Program Association Table
    PAT pat( xport_stream_id, 0x01 );

    // override the default maximum section length.. (optional)
    pat.setMaxSectionLen( 900 );

    // add the services (programs).. first, set the network pid (required for
    // some systems)
    pat.addNetworkPid( network_pid );

    for (int i = 0; i < num_services; i++)
    {
	    // each service has an id and pmt pid associated with it..
        ui16 service_id = 100 + i;
        ui16 pmt_pid = 200 + i;

        pat.addProgram( service_id, pmt_pid );
    }

    // for debug output
    DUMP(pat);

    // this builds the binary section data onto the TStream object
    pat.buildSections(t);



    //
    // create a DVB Time and Date Table with the current time
    TDT tdt;
    DUMP(tdt);     // debug output to stderr to see table data

    // write table data onto the TStream
    tdt.buildSections(t);




    // we now have a TStream that holds a list<Section *>..

    DUMP(t);         // debug - dump the contents of the TStream binary data

    // each Section holds an ui8[] with the data. we can write it to an
    // ostream (file, etc) by using the write method defined by
    // Section. Alternatively, you can have direct access to the binary
    // data by invoking getBinaryData().. regardless, iterate throguh
    // the list and do whatever you need like so:
    for (std::list<Section *>::const_iterator s_iter = t.section_list.begin();
	     s_iter != t.section_list.end();
	     s_iter++)
    {
       // const Section& section = *(*s_iter);
       // const ui8* sec_data = section.getBinaryData();
    }

```


Known Problems
==============

There were a few missing DVB descriptors that needed to be
implemented. Aside from that, this was used across several broadcast
systems and various testing environments.
