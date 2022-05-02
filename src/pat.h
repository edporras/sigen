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
// pat.h: class definition for the PAT
// -----------------------------------

#pragma once

#include <list>
#include "table.h"

namespace sigen {

   /*! \addtogroup table
    *  @{
    */

   /*! \addtogroup MPEG
    *  @{
    */

   /*!
    * \brief Program Association %Table, as per ISO 13818-1.
    */
   class PAT : public PSITable
   {
   public:
      enum { PID = 0x00 };

      /*!
       * \brief Constructor.
       * \param xs_id Unique id of the transport stream.
       * \param version_number Version number to use the subtable.
       * \param current_next_indicator `true`: version curently applicable, `false`: next applicable.
       */
      PAT(ui16 xs_id, ui8 version_number, bool current_next_indicator = true)
         : PSITable(TID, xs_id, 5, MAX_SEC_LEN, version_number, current_next_indicator, D_BIT)
      { }

      /*!
       * \brief Add an entry to the program loop associating the
       * specified pid.
       * \param program_number Identifes the program.
       * \param program_map_pid PID of the transport stream packets to contain the PMT for the specified program.
       */
      bool addProgram(ui16 program_number, ui16 program_map_pid);

      /*!
       * \brief Convenience method to add a network pid (setting
       *        program number as 0).
       * \param network_pid Pid of the transport stream carrying the NIT.
       */
      bool addNetworkPid(ui16 network_pid) {
         return addProgram(0, network_pid);
      }

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   private:
      enum { D_BIT = 0,
             TID = 0x00,
             MAX_SEC_LEN = 1024 };

      // the program / pid holder struct - private to the pat
      struct Program : public STable::ListItem {
         enum { BASE_LEN = 4 };

         ui16 number;
         ui16 pid : 13;

         // constructor
         Program(ui16 n, ui16 p) : number(n), pid(p) {}
         Program() = delete;
      };

      // the list of program / pids
      std::list<Program> program_list;

      enum State_t { INIT, WRITE_HEAD, GET_PROGRAM, WRITE_PROGRAM };
      mutable struct Context {
         Context() : op_state(INIT), p(nullptr) {}

         State_t op_state;
         const Program *p;
         std::list<Program>::const_iterator p_iter;
      } run;

   protected:
      virtual bool writeSection(Section&, ui8, ui16 &) const;
   };
   //! @}
   //! @}
}
