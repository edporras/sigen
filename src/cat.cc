//
// cat.cc: class definition for the CAT
// -----------------------------------

#include <iostream>
#include <list>
#include "table.h"
#include "cat.h"
#include "tstream.h"
#include "descriptor.h"

namespace sigen
{
    //
    // add a descriptor to the list
    bool CAT::addDesc(Descriptor &d)
    {
        // make sure we have enough room to add it
        if ( !incLength( d.length() ) )
            return false;

        desc_list.push_back( PtrWrapper<Descriptor>(&d) );
        return true;
    }

    //
    // writes the data to the stream
    //
    bool CAT::writeSection(Section& section, ui8 cur_sec, ui16 &sec_bytes) const
    {
        enum State_t {
            WRITE_HEAD,
            GET_DESC, WRITE_DESC
        };

        bool done, exit;
        static bool d_done = false;
        static State_t op_state = WRITE_HEAD;
        static const Descriptor *d = NULL;
        static std::list<PtrWrapper<Descriptor> >::const_iterator d_iter = desc_list.begin();

        // associate the iterators to the list
        if (!d_done)
            if (!d)
                d_iter = desc_list.begin();

        done = exit = false;

        while (!exit)
        {
            switch (op_state)
            {
              case WRITE_HEAD:
                  // common data for every section
                  // if table's length is > max_section_len, we'll
                  // overwrite the section length later on
                  writeSectionHeader(section);

                  // section count information
                  section.set08Bits(cur_sec);

                  // we don't know the last section number yet, so
                  // for now set it to 0,
                  section.set08Bits(0);//last_sec_num);

                  sec_bytes = BASE_LEN; // the minimum section size
                  op_state = (!d ? GET_DESC : WRITE_DESC);
                  break;

              case GET_DESC:
                  if (!d_done)
                  {
                      // fetch the next descriptor
                      if (d_iter != desc_list.end())
                      {
                          d = (*d_iter++)();

                          // check if we can fit it in this section
                          if (sec_bytes + d->length() > getMaxDataLen())
                          {
                              // we can't.. return so we can get a new section
                              // we'll add it when we come back
                              op_state = WRITE_HEAD;
                              exit = true;
                          }
                          else // we found one to write
                              op_state = WRITE_DESC;
                          break;
                      }
                      else
                      {
                          // no more descriptors so all sections are done!
                          d = NULL;
                          d_done = false;
                          op_state = WRITE_HEAD;
                          exit = done = true;
                      }
                  }
                  break;

              case WRITE_DESC:
                  // add the network descriptor
                  d->buildSections(section);
                  sec_bytes += d->length();

                  // try to add another one
                  op_state = GET_DESC;
                  break;
            }
        }
        return done;
    }


#ifdef ENABLE_DUMP
    //
    // debug dump
    void CAT::dump(std::ostream &o) const
    {
        // table header
        dumpHeader( o, CAT_DUMP_S, RESERVED_S );
        o << std::endl;

        // program desc list
        dumpDescLoop( desc_list, o );
    }
#endif

} // namespace sigen
