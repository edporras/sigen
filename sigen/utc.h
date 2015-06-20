//
// utc.h: class definition for the UTC
// -----------------------------------

#ifndef SIGEN_UTC_H
#define SIGEN_UTC_H

#include <sigen/types.h>

namespace sigen {

    //
    // BCD time class
    class BCDTime
    {
    public:
        // use these for indexing the time arrays
        enum { HOUR_IDX, MINUTES_IDX, SECONDS_IDX, TIME_LEN };

        //
        // constructors
        // pass values in HEX!!
        BCDTime(ui8 h = 0, ui8 m = 0, ui8 s = 0) : hour(h), min(m), sec(s) { }
        // pass a bcd time array
        BCDTime(ui8 *bta);

        // accessors
        ui8 getHour() const { return hour; }
        ui8 getMinute() const { return min; }
        ui8 getSecond() const { return sec; }
        ui8 getBCDHour() const;
        ui8 getBCDMinute() const;
        ui8 getBCDSecond() const;

        // utility
        void set(ui8 h, ui8 m, ui8 s) { hour = h; min = m; sec = s; }

        // assign a bcd array
        BCDTime& operator=(const ui8* bta);

        // comparison op
        int operator==(const BCDTime &bcdt) const;

        // dump to an ostream
        friend std::ostream &operator<<(std::ostream& o, const BCDTime& t) {
            return t.dump(o);
        }

    private:
        ui8 hour,
            min,
            sec;

    protected:
        std::ostream& dump(std::ostream& o) const;
    };


    //
    // Universal Time Code
    //
    class UTC
    {
    public:
        // public data holders - for simplicity
        ui16 mjd;
        BCDTime time;

        // default constructor builds a UTC object with current time
        UTC();
        // these constructors must take h, m, s in HEX!!.. don't
        // pass BCD to construct
        UTC(ui16 mjd, ui8 h, ui8 m = 0, ui8 s = 0);
        UTC(ui16 M, ui16 D, ui16 Y, ui8 h, ui8 m = 0, ui8 s = 0);
        // this constructor DOES take mjd + bcd[3] (hr = 0, min = 1, sec = 2)
        UTC(ui16 mjd, ui8 bcd_time[BCDTime::TIME_LEN]);

        // accessors
        void getMDY(ui16 &M, ui16 &D, ui16 &Y) const;

        // comparison op
        int operator==(const UTC &) const;

        // overloaded << op
        friend std::ostream &operator<<(std::ostream &o, const UTC &t) {
            return t.dump(o);
        }

    protected:
        std::ostream& dump(std::ostream& o) const;
    };

} // sigen namespace

#endif
