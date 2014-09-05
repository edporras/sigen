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


#if 0

    /* some handy constant bcd times */
    ui8 one_sec[TIME_LEN] = { 0x00, 0x00, 0x01 };

    /* days per month */
    ui8 dpm[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    /* ------------------------------------------------------------------------- */
    /* UTCPlusDur takes a time_st (mjd + bcd time) and duration (in bcd),        */
    /* and computes the resulting utc into result.. orig and result can point to */
    /* the same struct since a copy of the data is made.                         */
    /* If the time spans across midnight, then extra effort is made to update    */
    /* the mjd, otherwise the original mjd is copied to it                       */
    /* ------------------------------------------------------------------------- */
    BOOLEAN UTCPlusDur(time_st *orig, ui8 *bcddur, time_st *result)
    {
        int i, daycnt;
        ui16 month1, day1, year1, month2, day2, year2;
        ui8 time2[TIME_LEN], time1[TIME_LEN], dur[TIME_LEN];
        div_t delta;

        /* convert from bcd to hex */
        for (i = 0; i < TIME_LEN; i++)
        {
            time1[i] = bcd2hex[orig->time[i]];
            dur[i] = bcd2hex[bcddur[i]];
        }

        /* calculate seconds - fix minutes */
        time2[T_SECOND] = time1[T_SECOND] + dur[T_SECOND];
        if (time2[T_SECOND] > 59)
        {
            delta = div(time2[T_SECOND], 60);
            time2[T_SECOND] = delta.rem;
            time2[T_MINUTE] = time1[T_MINUTE] + dur[T_MINUTE] + delta.quot;
        }
        else
            time2[T_MINUTE] = time1[T_MINUTE] + dur[T_MINUTE];

        /* calculate minutes - fix hours..*/
        if (time2[T_MINUTE] > 59)
        {
            delta = div(time2[T_MINUTE], 60);
            time2[T_MINUTE] = delta.rem;
            time2[T_HOUR] = time1[T_HOUR] + dur[T_HOUR] + delta.quot;
        }
        else
            time2[T_HOUR] = time1[T_HOUR] + dur[T_HOUR];

        /* if the hour did not span accross midnight, we copy mjd1 to mjd2
           and we're done, baby! */
        if (time2[T_HOUR] <= 23)
            result->date = orig->date;
        else
        {
            /* otherwise, we have a to do some extra work to convert MJD to earthling
               date, calculate the new date, and then convert back to MJD */
            MJD2Date(orig->date, &month1, &day1, &year1);

            /* fix hours accordingly */
            delta = div(time2[T_HOUR], 24);
            time2[T_HOUR] = delta.rem;
            day2 = day1 + delta.quot;

            daycnt = dpm[month1 - 1];
            if (month1 == 2) /* if feb, check leap year */
            {
                if (IsLeapYear(year1))
                    daycnt++;
            }

            /* fix days accordingly */
            if (day2 > daycnt)
            {
                delta = div(day2, daycnt);

                day2 = delta.rem;
                month2 = month1 + delta.quot;
            }
            else
                month2 = month1;

            /* fix months accordingly */
            if (month2 > 12)
            {
                delta = div(month2, 12);

                month2 = delta.rem;
                year2 = year1 + delta.quot;
            }
            else
                year2 = year1;

            result->date = Date2MJD(month2, day2, year2);
        }

        /* convert it back to bcd */
        for (i = 0; i < TIME_LEN; i++)
            result->time[i] = hex2bcd[time2[i]];

        return true;
    }


    //
    // UTCMinusDur takes a time_st (mjd + bcd time) and duration (in bcd),
    // and computes the resulting utc into result.. orig and result can point to
    // the same struct since a copy of the data is made.
    // If the time spans across midnight, then extra effort is made to update
    // the mjd, otherwise the original mjd is copied to it
    //
    BOOLEAN UTCMinusDur(time_st *orig, ui8 *bcddur, time_st *result)
    {
        int i, daycnt;
        ui16 month1, day1, year1, month2, day2, year2;
        i8 t2[TIME_LEN];
        ui8 time1[TIME_LEN], dur[TIME_LEN];

        /* convert bcd to hex */
        for (i = 0; i < TIME_LEN; i++)
        {
            time1[i] = bcd2hex[orig->time[i]];
            dur[i] = bcd2hex[bcddur[i]];
        }

        /* calculate seconds.. */
        t2[T_SECOND] = time1[T_SECOND] - dur[T_SECOND];
        if (t2[T_SECOND] < 0)
        {
            t2[T_SECOND] += 60;
            t2[T_MINUTE] = time1[T_MINUTE] - dur[T_MINUTE] - 1;
        }
        else
            t2[T_MINUTE] = time1[T_MINUTE] - dur[T_MINUTE];

        /* calculate minutes */
        if (t2[T_MINUTE] < 0)
        {
            t2[T_MINUTE] += 60;
            t2[T_HOUR] = time1[T_HOUR] - dur[T_HOUR] - 1;
        }
        else
            t2[T_HOUR] = time1[T_HOUR] - dur[T_HOUR];

        /* if the hour did not span accross midnight, we copy mjd1 to mjd2
           and we're done, baby! */
        if (t2[T_HOUR] >= 0)
            result->date = orig->date;
        else
        {
            /* otherwise, we have a to do some fourier transforms, et al. to convert
               MJD to human date, calculate the new date blah.. blah.. */
            MJD2Date(orig->date, &month1, &day1, &year1);

            t2[T_HOUR] += 24;
            day2 = day1 - 1;

            if (month1 - 2 <= 0)
                daycnt = dpm[11];
            else
                daycnt = dpm[month1 - 2];

            if (day2 <= 0)
            {
                if (month1 - 1 == 2)
                {
                    if (IsLeapYear(year1))
                        daycnt++;
                }
                day2 = daycnt + day2;
                month2 = month1 - 1;
            }
            else
                month2 = month1;

            if (month2 <= 0)
            {
                month2 += 12;
                year2 = year1 - 1;
            }
            else
                year2 = year1;

            result->date = Date2MJD(month2, day2, year2);
        }

        /* convert it back to bcd */
        for (i = 0; i < TIME_LEN; i++)
            result->time[i] = hex2bcd[t2[i]];

        return true;
    }


    //
    // handy functions
    //
    ui16 BCDToMins( ui8 * bcd_time )
    {
        return (bcd2hex[bcd_time[T_HOUR]] * 60 + bcd2hex[bcd_time[T_MINUTE]]);
    }


    // NOTE: must pass in LOCAL mjd from the current_time struct held by
    // supvtime.c
    void Date2NASP(ui16 *local_mjd, ui8 *nasp_date)
    {
        ui16 m,d,y;
        ui16 temp_date;

        MJD2Date(*local_mjd, &m,&d,&y);

        y -= 1992;	    /* conversion for nasp format */

        temp_date  = (y&0x007F)<<9;
        temp_date |= (m&0x0F)<<5;

        temp_date |= (d&0x1F);

        nasp_date[0] = (temp_date>>8)&0x00FF;
        nasp_date[1] = temp_date&0x00FF;
    }

    /* eof */
#endif

} // namespace sigen
