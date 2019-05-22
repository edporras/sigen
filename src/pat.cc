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
// pat.cc: class definition for the PAT
// -----------------------------------

#include <iostream>
#include <utility>
#include <list>
#include "table.h"
#include "pat.h"
#include "tstream.h"

namespace sigen
{
   //
   // adds a program to the pat
   bool PAT::addProgram(ui16 sid, ui16 pid)
   {
      // make sure we can add it
      if ( !incLength(Program::BASE_LEN) )
         return false;

      program_list.push_back( std::unique_ptr<Program>(new Program(sid, pid)) );
      return true;
   }


   //
   // writes to the stream
   bool PAT::writeSection(Section& section, ui8 cur_sec, ui16 &sec_bytes) const
   {
      enum State_t { WRITE_HEAD, GET_PROGRAM, WRITE_PROGRAM };

      bool done, exit;
      static State_t op_state = WRITE_HEAD;
      static const Program *p = nullptr;
      static std::list<std::unique_ptr<Program> >::const_iterator p_iter = program_list.begin();

      // init
      done = exit = false;
      if (p_iter == program_list.end())
         p_iter = program_list.begin();

      while (!exit)
      {
         switch (op_state)
         {
           case WRITE_HEAD:
              // common data for every section
              // if table's length is > available space, we'll
              // overwrite the section length later on
              writeSectionHeader(section);

              // section count information
              section.set08Bits(cur_sec);

              // we don't know the last section number yet, so
              // for now set it to 0,
              section.set08Bits(0);

              sec_bytes = BASE_LEN; // the minimum section size
              op_state = (!p ? GET_PROGRAM : WRITE_PROGRAM);
              break;

           case GET_PROGRAM:
              // fetch the next service
              if (p_iter != program_list.end())
              {
                 p = (*p_iter++).get();

                 // can we add it?
                 if ( (sec_bytes + Program::BASE_LEN) > getMaxDataLen() )
                 {
                    // no soup for you
                    op_state = WRITE_HEAD;
                    exit = true;
                    break;
                 }
                 // we can add it
                 op_state = WRITE_PROGRAM;
              }
              else
              {
                 // done with all programs.. all sections are done!
                 op_state = WRITE_HEAD;
                 p = nullptr;
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_PROGRAM:
              section.set16Bits(p->number);
              section.set16Bits( rbits(reserved, 0xe000) | p->pid );

              sec_bytes += Program::BASE_LEN;
              op_state = GET_PROGRAM;
              break;
         }
      }
      return done;
   }


#ifdef ENABLE_DUMP
   //
   // dumps to an ostream
   void PAT::dump(std::ostream &o) const
   {
      // table header
      dumpHeader( o, PAT_DUMP_S, XPORT_STREAM_ID_S, true );
      o << std::endl;

      // program list
      incOutLevel(); // indent output
      for ( std::list<std::unique_ptr<Program> >::const_iterator p_iter = program_list.begin();
            p_iter != program_list.end();
            p_iter++ )
      {
         const Program& program = *(*p_iter);

         identStr(o, PROGRAM_NUM_S, program.number, true);
         identStr(o, RESERVED_S, rbits(reserved, 0x07) );
         identStr(o, PID_S, program.pid, true);
         o << std::endl;
      }
      o << std::endl;
      decOutLevel();
   }
#endif

} // namespace sigen
