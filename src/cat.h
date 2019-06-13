// Copyright 1999-2019 Ed Porras
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
// cat.h: class definition for the CAT
// -----------------------------------

#pragma once

#include "table.h"
#include "util.h"

namespace sigen {

   class Descriptor;

   //
   // the cat class
   //
   class CAT : public PSITable
   {
   public:
      enum { PID = 0x01 };

      // constructor
      CAT(ui8 ver, bool cni = true) :
         PSITable(TID, rbits(0xffff), BASE_LEN, MAX_SEC_LEN,
                  ver, cni, D_BIT)
      { }
      CAT() = delete;

      // utility
      bool addDesc(Descriptor &);

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   private:
      enum { D_BIT = 0,
             TID = 0x01,
             BASE_LEN = 5, MAX_SEC_LEN = 1024 };

      // list of descriptors
      DescList descriptors;

      enum State_t { INIT, WRITE_HEAD, GET_DESC, WRITE_DESC };
      mutable struct Context {
         Context() : d_done(false), op_state(INIT), d(nullptr) {}

         bool d_done;
         State_t op_state;
         const Descriptor *d;
         std::list<std::unique_ptr<Descriptor> >::const_iterator d_iter;
      } run;

   protected:
      virtual bool writeSection(Section&, ui8, ui16 &) const;
   };

} // sigen namespace
