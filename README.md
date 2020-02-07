C++ Si Generation Library 2.8.1
===============================

A C++ [MPG PSI](https://en.wikipedia.org/wiki/Program-specific_information) and
[DVB SI](https://en.wikipedia.org/wiki/Digital_Video_Broadcasting)
table generation library based on the ETSI EN 300 468 and ISO/IEC
13818-1 standards. The library has support most tables: CAT, PAT, PMT,
NIT, SDT, BAT, TDT, TOT, RST, and ST plus many MPG and DVB descriptors.

I wrote the initial version of this library ca. 1999-2000 and updated
it around 2003 with a few minor tweaks done since then. It was used in
production but nobody had since been maintaining it so I was given
permission to open the source so I could make changes to it if
needed.

A Doxygen-generated reference [is available](http://edporras.github.io/sigen/docs/).

This is very legacy so now it's mainly a C++ feature playground. Also,
some of the logic could definitely be re-thought as it was done based
on customer requirements.


Building
========

```
  autoreconf -i
  ./configure
  make
```

Sample Usage
============

See tests/*.cc for more thorough examples of building various tables. Here's a snippet:

```c++

    // some dummy data
    const ui16 network_pid = 0x20;
    const ui16 xport_stream_id = 0x10;
    const int num_services = 10;

    // the binary table data will be written to a TStream
    TStream t;

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

    //
    // we now have a TStream that holds a list<Section *>..

    DUMP(t);       // debug - dump the contents of the TStream binary data

    // you can write it to disk, packetize it, etc
    t.write("tables.ts");
```

Program output:


```
- - PAT Dump - -
Table id            : 0
Section syn. ind.   : 0x1
rsvrd. future use   : 0
reserved            : 0x3
Table length        : 49 (0x31)
Xport stream id     : 16 (0x10)
reserved            : 0x3
Version number      : 0x1
Current next ind.   : 0x1

 Program number      : 0 (0)
 reserved            : 0x7
 Pid                 : 32 (0x20)

 Program number      : 100 (0x64)
 reserved            : 0x7
 Pid                 : 200 (0xc8)

 Program number      : 101 (0x65)
 reserved            : 0x7
 Pid                 : 201 (0xc9)

 Program number      : 102 (0x66)
 reserved            : 0x7
 Pid                 : 202 (0xca)

 Program number      : 103 (0x67)
 reserved            : 0x7
 Pid                 : 203 (0xcb)

 Program number      : 104 (0x68)
 reserved            : 0x7
 Pid                 : 204 (0xcc)

 Program number      : 105 (0x69)
 reserved            : 0x7
 Pid                 : 205 (0xcd)

 Program number      : 106 (0x6a)
 reserved            : 0x7
 Pid                 : 206 (0xce)

 Program number      : 107 (0x6b)
 reserved            : 0x7
 Pid                 : 207 (0xcf)

 Program number      : 108 (0x6c)
 reserved            : 0x7
 Pid                 : 208 (0xd0)

 Program number      : 109 (0x6d)
 reserved            : 0x7
 Pid                 : 209 (0xd1)


- - TDT Dump - -
Table id            : 0x70
Section syn. ind.   : 0
rsvrd. future use   : 0x1
reserved            : 0x3
Table length        : 5 (0x5)
UTC                 : 0xdf69 (6/20/2015), 19:00:17

-- Section dump - num_sections: 2 --

- sec: 0, length: 56, size (max): 900, data:
[0000] 00 b0 35 00 10 c3 00 00 00 00 e0 20 00 64 e0 c8   ..5........ .d..
[0016] 00 65 e0 c9 00 66 e0 ca 00 67 e0 cb 00 68 e0 cc   .e...f...g...h..
[0032] 00 69 e0 cd 00 6a e0 ce 00 6b e0 cf 00 6c e0 d0   .i...j...k...l..
[0048] 00 6d e0 d1 4f 73 46 ef                           .m..OsF.

- sec: 1, length: 8, size (max): 8, data:
[0000] 70 70 05 df 69 19 00 17                           pp..i...

```


Known Problems
==============

There are a few descriptors that needed to be implemented - see
[TODO](TODO). They were not required by the customer so they never
made it in. Regardless, this library was used on at least two
production systems (N.A. and Spain) and to generate tables for
various testing environments.
