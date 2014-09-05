//
// pat.h: class definition for the PAT
// -----------------------------------

#ifndef SIGEN_PAT_H
#define SIGEN_PAT_H

#include <list>
#include <sigen/table.h>
#include <sigen/util.h>

namespace sigen {

    //
    // the pat class
    //
    class PAT : public PSITable
    {
    private:
        enum { D_BIT = 0,
               TID = 0x00,
               BASE_LEN = 5, MAX_SEC_LEN = 1024 };

        // the program / pid holder struct - private to the pat
        struct Program {
            enum { BASE_LEN = 4 };

            ui16 number,
                pid : 13;

            // constructor
            Program(ui16 n, ui16 p) : number(n), pid(p) {}

            Program* clone() const { return new Program(*this); }
        };

        // the list of program / pids
        std::list< PtrWrapper<Program> > program_list;

    public:
        enum { PID = 0x00 };

        // constructor
        PAT(ui16 xport_str_id, ui8 ver, bool cni = true, bool rsrvd = true) :
            PSITable(TID, xport_str_id, BASE_LEN, MAX_SEC_LEN, ver, cni,
                     D_BIT, rsrvd)
        { }

        // utility
        bool addProgram(ui16 sid, ui16 pid);
        bool addNetworkPid(ui16 pid) { return addProgram(0, pid); }

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream &) const;
#endif

    protected:
        virtual bool writeSection(Section&, ui8, ui16 &) const;
    };

} // sigen namespace

#endif
