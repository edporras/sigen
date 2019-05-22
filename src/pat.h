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
// pat.h: class definition for the PAT
// -----------------------------------

#pragma once

#include <list>
#include <memory>
#include "table.h"
#include "util.h"

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
      };

      // the list of program / pids
      std::list<std::unique_ptr<Program> > program_list;

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
