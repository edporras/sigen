//
// cat.h: class definition for the CAT
// -----------------------------------

#ifndef SIGEN_CAT_H
#define SIGEN_CAT_H

#include <list>
#include <sigen/table.h>
#include <sigen/util.h>

namespace sigen {

    class Descriptor;

    //
    // the cat class
    //
    class CAT : public PSITable
    {
    private:
        enum { D_BIT = 0,
               TID = 0x01,
               BASE_LEN = 5, MAX_SEC_LEN = 1024 };

        // list of descriptors
        std::list<PtrWrapper<Descriptor> > desc_list;

    public:
        enum { PID = 0x01 };

        // constructor
        CAT(ui8 ver, bool cni = true, bool rsrvd = true) :
            PSITable(TID, rbits(rsrvd, 0xffff), BASE_LEN, MAX_SEC_LEN,
                     ver, cni, D_BIT, rsrvd)
        { }

        // utility
        bool addDesc(Descriptor &);

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream &) const;
#endif

    protected:
        virtual bool writeSection(Section&, ui8, ui16 &) const;
    };

} // sigen namespace

#endif
