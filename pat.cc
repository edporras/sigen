//
// pat.cc: class definition for the PAT
// -----------------------------------

#include <iostream>
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

        program_list.push_back( PtrWrapper<Program>(new Program(sid, pid)) );
        return true;
    }


    //
    // writes to the stream
    bool PAT::writeSection(Section& section, ui8 cur_sec, ui16 &sec_bytes) const
    {
        enum State_t { WRITE_HEAD, GET_PROGRAM, WRITE_PROGRAM };

        bool done, exit;
        static State_t op_state = WRITE_HEAD;
        static const Program *p = NULL;
        static std::list<PtrWrapper<Program> >::const_iterator p_iter = program_list.begin();

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
                      p = (*p_iter++)();

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
                      p = NULL;
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
        for ( std::list<PtrWrapper<Program> >::const_iterator p_iter = program_list.begin();
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
