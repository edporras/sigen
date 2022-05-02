// Copyright 1999-2022 Ed Porras
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// utc.cc: class definition for the UTC
// -----------------------------------

#include <ctime> // unix
#include <iostream>
#include <iomanip>
#include "utc.h"

namespace sigen
{
   namespace UTC_priv {
      //
      // bcd <-> hex arrays
      static const ui8 BCD2HEX[97] = {
         0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, //  0 -  9
         0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x0a, 0x0b, 0x0c, 0x0d, // 10 - 19
         0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, // 20 - 29
         0x18, 0x19, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, // 30 - 39
         0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x1e, 0x1f, // 40 - 49
         0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, // 50 - 59
         0x2a, 0x2b, 0x2c, 0x2d, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, // 60 - 69
         0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, // 70 - 79
         0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, // 80 - 89
         0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x3c                    // 90 - 96
      };

      static const ui8 HEX2BCD[61] = {
         0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, //  0 -  9
         0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, // 10 - 19
         0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, // 20 - 29
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, // 30 - 39
         0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, // 40 - 49
         0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, // 50 - 59
         0x60                                                        // 60
      };

      const ui8 *bcd2hex = &BCD2HEX[0];
      const ui8 *hex2bcd = &HEX2BCD[0];

      void getSysTime(ui16 &M, ui16 &D, ui16 &Y, ui8 &h, ui8 &m, ui8 &s);
      ui16 date2MJD(ui16 M, ui16 D, ui16 Y);
      void MJD2Date(ui16 mjd, ui16 &M, ui16 &D, ui16 &Y);
   };

   using namespace UTC_priv;


   // ----------------------------------------------------------------------
   // gets the system time
   //
   void UTC_priv::getSysTime(ui16 &M, ui16 &D, ui16 &Y, ui8 &h, ui8 &m, ui8 &s)
   {
      // gcc / unix system time calls
      tm *t;
      time_t utc_s;

      time(&utc_s);
      t = localtime(&utc_s);

      M = t->tm_mon + 1; // tm_mon is 0 indexed
      D = t->tm_mday;
      Y = t->tm_year;
      h = t->tm_hour;
      m = t->tm_min;
      s = t->tm_sec;
   }

   // ----------------------------------------------------------------------
   // global conversion routines mjd <-> M D Y
   //
   ui16 UTC_priv::date2MJD(ui16 M, ui16 D, ui16 Y)
   {
      ui16 mjd, L = 0;

      // remove the leading '19'
      if (Y > 1900)
         Y -= 1900;

      // from the formula in pg 71 ETS 300 468 jan 96
      if (M == 1 || M == 2)
         L = 1;

      mjd = 14956 + (ui16) D +
         (ui16) (((float) Y - (float) L) * (float) 365.25) +
         (ui16) (((float) M + (float) 1 + (float) L * (float) 12) *
                 (float) 30.6001);
      return mjd;
   }

   void UTC_priv::MJD2Date(ui16 mjd, ui16 &M, ui16 &D, ui16 &Y)
   {
      ui16 mo, yr, K = 0;
      double Mp;

      const double DPY = 365.25;

      // parse the MJD.. do the math
      yr = (ui16) ((mjd - 15078.2) / DPY);
      Mp = mjd - 14956.1 - (ui16)(yr * DPY);
      mo = (ui16) (Mp / 30.6001);
      D = mjd - 14956 - (ui16)(yr * DPY) - (ui16)(mo * 30.6001);

      if (mo == 14 || mo == 15)
         K = 1;

      Y = yr + K + 1900;
      M = mo - 1 - K * 12;
   }


   // ---------------------------------------
   // BCD Time class
   //

   //
   // convert constructor - takes a 3-byte bcd array
   //
   BCDTime::BCDTime(ui8 *bta) {
      set( bcd2hex[ bta[ HOUR_IDX ] ],
           bcd2hex[ bta[ MINUTES_IDX ] ],
           bcd2hex[ bta[ SECONDS_IDX ] ]);
   }

   // getters
   ui8 BCDTime::getBCDHour() const { return hex2bcd[ hour ]; }
   ui8 BCDTime::getBCDMinute() const { return hex2bcd[ min ]; }
   ui8 BCDTime::getBCDSecond() const { return hex2bcd[ sec ]; }

   // assignment op
   BCDTime &BCDTime::operator=(const ui8 *bta) {
      set( bcd2hex[ bta[HOUR_IDX] ], bcd2hex[ bta[MINUTES_IDX] ],
           bcd2hex[ bta[SECONDS_IDX] ]);
      return *this;
   }

   //
   // comparison op
   //
   int BCDTime::operator==(const BCDTime &t) const
   {
      if (hour < t.hour)
         return -1;
      if (hour > t.hour)
         return 1;
      if (min < t.min)
         return -1;
      if (min > t.min)
         return 1;
      if (sec < t.sec)
         return -1;
      if (sec > t.sec)
         return 1;
      return 0;
   }


   //
   // dumps the contents to an ostream
   std::ostream& BCDTime::dump(std::ostream& o) const
   {
      std::ios::fmtflags f = o.flags();
      o.unsetf(std::ios::showbase);
      o.setf(std::ios::right);
      o << std::hex << std::setfill('0')
        << std::setw(2) << (ui16) hex2bcd[ hour ] << ":"
        << std::setw(2) << (ui16) hex2bcd[ min ] << ":"
        << std::setw(2) << (ui16) hex2bcd[ sec ];
      o.flags(f);
      return o;
   }


   // ---------------------------------------
   // UTC class
   //

   //
   // constructors / destructor
   UTC::UTC()
   {
      ui16 M, D, Y;
      ui8 h, m, s;

      getSysTime(M, D, Y, h, m, s);
      mjd = date2MJD(M, D, Y);
      time.set(h, m, s);
   }

   UTC::UTC(ui16 mjd_, ui8 h, ui8 m, ui8 s) :
      mjd(mjd_)
   {
      time.set(h, m, s);
   }


   UTC::UTC(ui16 M, ui16 D, ui16 Y, ui8 h, ui8 m, ui8 s) :
      mjd( date2MJD(M, D, Y) )
   {
      time.set(h, m, s);
   }


   UTC::UTC(ui16 date, ui8 bcd_time[BCDTime::TIME_LEN]) :
      mjd(date),
      time(bcd_time)
   {
   }


   //
   // sets the values of M, D, Y based on mjd
   void UTC::getMDY(ui16 &M, ui16 &D, ui16 &Y) const {
      MJD2Date(mjd, M, D, Y);
   }


   //
   // compare two date/time pairs
   //
   int UTC::operator==(const UTC &t) const
   {
      // compare the dates
      if (mjd > t.mjd)
         return 1;
      if (mjd < t.mjd)
         return -1;

      // compare the time arrays: 0 = HOURS, 1 = MINUTES, 2 = SECONDS
      return (time == t.time);
   }


   //
   // dumps the utc
   std::ostream& UTC::dump(std::ostream& o) const
   {
      ui16 M, D, Y;

      getMDY(M, D, Y);

      // save the current state of the stream
      std::ios::fmtflags f = o.flags();

      // format the UTC's output
      o.setf(std::ios::right);
      o << std::setfill('0')
        << std::hex << mjd
        << " ("
        << std::dec << M << "/" << D << "/" << Y
        << "), "
        << time;

      // restore the state
      o.flags(f);
      return o;
   }
} // namespace sigen
