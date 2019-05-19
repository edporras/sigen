//
// pmt.cc: class definition for the PMT
// -----------------------------------

#include <iostream>
#include <list>
#include "table.h"
#include "pmt.h"
#include "descriptor.h"
#include "tstream.h"

namespace sigen
{
   //
   // add a descriptor to the table
   bool PMT::addProgramDesc(Descriptor &d)
   {
      ui16 d_len = d.length();
      if ( !incLength(d_len) )
         return false;

      desc_list.push_back( PtrWrapper<Descriptor>(&d) );
      program_info_length += d_len;
      return true;
   }

   //
   // add an elementary stream
   bool PMT::addElemStream(ui8 type, ui16 elem_pid)
   {
      if ( !incLength(ElementaryStream::BASE_LEN) )
         return false;

      ElementaryStream *stream = new ElementaryStream(elem_pid, type);
      es_list.push_back( PtrWrapper<ElementaryStream>(stream) );
      return true;
   }


   //
   // add a descriptor to the stream specified by elem_pid
   //
   bool PMT::addElemStreamDesc(ui16 elem_pid, Descriptor &d)
   {
      ui16 d_len = d.length();
      if ( !incLength( d_len ) )
         return false;

      // look for the stream
      for ( std::list<PtrWrapper<ElementaryStream> >::iterator s_iter = es_list.begin();
            s_iter != es_list.end();
            s_iter++ )
      {
         ElementaryStream& stream = *(*s_iter);

         if (stream.elementary_pid == elem_pid)
            return addElemStreamDesc(stream, d, d_len);
      }
      return false;
   }

   //
   // tries to add a descriptor to the last stream added
   //
   bool PMT::addElemStreamDesc(Descriptor &d)
   {
      ui16 d_len = d.length();
      if ( !incLength( d_len ) )
         return false;

      // grab the stream at the back (if there's one)
      if (es_list.size() > 0)
         return addElemStreamDesc( *(es_list.back()), d, d_len );
      return false;
   }

   //
   // actually adds the descriptor to the transport stream
   //
   bool PMT::addElemStreamDesc(ElementaryStream& stream, Descriptor &d, ui16 d_len)
   {
      stream.desc_list.push_back( PtrWrapper<Descriptor>(&d) );
      stream.es_info_length += d_len;
      return true;
   }

   //
   // writes the data to the stream
   //
   bool PMT::writeSection(Section& section, ui8 cur_sec, ui16 &sec_bytes) const
   {
      enum State_t {
         WRITE_HEAD,
         GET_PROG_DESC, WRITE_PROG_DESC,
         GET_XPORT_STREAM, WRITE_XPORT_STREAM
      };

      ui8 *prog_info_len_pos = 0;
      ui16 d_len, prog_info_len = 0;
      bool done, exit;
      static bool d_done = false;
      static State_t op_state = WRITE_HEAD;
      static const Descriptor *pd = NULL;
      static const ElementaryStream *ts = NULL;
      static std::list<PtrWrapper<Descriptor> >::const_iterator pd_iter = desc_list.begin();
      static std::list<PtrWrapper<ElementaryStream> >::const_iterator ts_iter = es_list.begin();

      // associate the iterators to the list
      if (!d_done)
         if (!pd)
            pd_iter = desc_list.begin();

      if (!ts)
         ts_iter = es_list.begin();

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

              section.set16Bits( rbits(reserved, 0xe000) | pcr_pid );

              // save the position for the network_desc_loop_len
              prog_info_len_pos = section.getCurDataPosition();

              // for now, set it to something.. this is necessary to
              // increment the internal data pointer of the buffer
              section.set16Bits( 0 );

              sec_bytes = BASE_LEN; // the minimum section size
              op_state = (!pd ? GET_PROG_DESC : WRITE_PROG_DESC);
              break;

           case GET_PROG_DESC:
              // update the prog_info_len.. we always do this
              // and if we don't add any, it is set to 0 anyways
              section.set16Bits(prog_info_len_pos,
                                rbits( reserved, ~LEN_MASK) |
                                (prog_info_len & LEN_MASK) );

              if (!d_done)
              {
                 // fetch the next program descriptor
                 if (pd_iter != desc_list.end())
                 {
                    pd = (*pd_iter++)();

                    // check if we can fit it in this section
                    if (sec_bytes + pd->length() > getMaxDataLen())
                    {
                       // we can't.. return so we can get a new section
                       // we'll add it when we come back
                       op_state = WRITE_HEAD;
                       exit = true;
                    }
                    else // we found one to write
                       op_state = WRITE_PROG_DESC;
                    break;
                 }
                 else
                 {
                    pd = NULL;
                    d_done = true;
                    // fall through
                 }
              }

              // done with all program descriptors.. move on to the
              // transport streams
              op_state = (!ts ? GET_XPORT_STREAM : WRITE_XPORT_STREAM);
              break;

           case WRITE_PROG_DESC:
              // add the network descriptor
              pd->buildSections(section);

              d_len = pd->length();
              sec_bytes += d_len;
              prog_info_len += d_len;

              // try to add another one
              op_state = GET_PROG_DESC;
              break;

           case GET_XPORT_STREAM:
              // fetch a transport stream
              if (ts_iter != es_list.end())
              {
                 ts = (*ts_iter++)();

                 // first, check if it has any descriptors.. we'll try to fit
                 // at least one
                 if (ts->desc_list.size() > 0)
                 {
                    const Descriptor *d = (*ts->desc_list.begin())();

                    // check the size with the descriptor
                    if ( (sec_bytes + PMT::ElementaryStream::BASE_LEN + d->length()) >
                         getMaxDataLen() )
                    {
                       // won't fit.. wait until the next section
                       op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 else
                 {
                    // check if this XS with no descs will fit here
                    if ( (sec_bytes + PMT::ElementaryStream::BASE_LEN) > getMaxDataLen() )
                    {
                       // nope.. wait also
                       op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 // all is ok.. add it
                 op_state = WRITE_XPORT_STREAM;
              }
              else
              {
                 // no more network descriptors or transport streams, so
                 // all sections are done!
                 ts = NULL;
                 d_done = false;
                 op_state = WRITE_HEAD;
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_XPORT_STREAM:
              // finally write it
              if (!writeStream(section, *ts, sec_bytes))
              {
                 op_state = WRITE_HEAD;
                 exit = true;
                 break;
              }
              op_state = GET_XPORT_STREAM;
              break;
         }
      }
      return done;
   }


   //
   // state machine for writing the transport streams
   //
   bool PMT::writeStream(Section& section, const ElementaryStream& stream,
                         ui16 &sec_bytes) const
   {
      enum State_t { WRITE_HEAD, GET_DESC, WRITE_DESC };

      static State_t op_state = WRITE_HEAD;

      ui8 *ts_desc_len_pos = 0;
      ui16 d_len, ts_desc_len = 0;
      bool exit, done;
      static const Descriptor *tsd = NULL;
      static std::list<PtrWrapper<Descriptor> >::const_iterator tsd_iter = stream.desc_list.begin();

      exit = done = false;

      // set the descriptor iterator
      if (!tsd)
         tsd_iter = stream.desc_list.begin();

      while (!exit)
      {
         switch (op_state)
         {
           case WRITE_HEAD:
              // write the pmt transport stream data
              section.set08Bits(stream.type);
              section.set16Bits( rbits(reserved, 0xe000) |
                                 stream.elementary_pid );

              // save the position for the desc loop len.. we'll update it later
              ts_desc_len_pos = section.getCurDataPosition();
              section.set16Bits( 0 );

              // increment the byte count
              sec_bytes += ElementaryStream::BASE_LEN;

              op_state = (!tsd ? GET_DESC : WRITE_DESC);
              break;

           case GET_DESC:
              if (tsd_iter != stream.desc_list.end())
              {
                 tsd = (*tsd_iter++)();

                 // make sure we can fit it
                 if ( (sec_bytes + tsd->length()) > getMaxDataLen() )
                 {
                    op_state = WRITE_HEAD;
                    exit = true;
                    break;
                 }
                 op_state = WRITE_DESC;
              }
              else
              {
                 // no more descriptors.. done writing this xport stream,
                 tsd = NULL;
                 op_state = WRITE_HEAD;
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_DESC:
              tsd->buildSections(section);

              // increment all byte counts
              d_len = tsd->length();
              sec_bytes += d_len;
              ts_desc_len += d_len;

              // try to get another one
              op_state = GET_DESC;
              break;
         }
      }
      // write the desc loop length
      section.set16Bits( ts_desc_len_pos,
                         rbits(reserved, ~LEN_MASK) | (ts_desc_len & LEN_MASK) );
      return done;
   }


#ifdef ENABLE_DUMP
   //
   // dumps to an ostream
   void PMT::dump(std::ostream &o) const
   {
      // table header
      dumpHeader( o, PMT_DUMP_S, PROGRAM_NUM_S, true );

      // pmt-specific
      identStr(o, RESERVED_S, rbits(reserved, 0x07) );
      identStr(o, PCR_PID_S, pcr_pid);
      identStr(o, RESERVED_S, rbits(reserved, 0x0f) );
      identStr(o, PROGRAM_INFO_LEN_S, program_info_length, true);
      o << std::endl;

      // program desc list
      dumpDescLoop( desc_list, o );

      // stream list
      incOutLevel(); // indent output
      headerStr(o, STREAM_LIST_S, false);

      for ( std::list<PtrWrapper<ElementaryStream> >::const_iterator s_iter = es_list.begin();
            s_iter != es_list.end();
            s_iter++ )
      {
         const ElementaryStream& stream = *(*s_iter);

         identStr(o, STREAM_TYPE_S, stream.type, true);
         identStr(o, RESERVED_S, rbits(reserved, 0x07) );
         identStr(o, ELEM_PID_S, stream.elementary_pid, true);
         identStr(o, RESERVED_S, rbits(reserved, 0x0f) );
         identStr(o, ES_INFO_LEN_S, stream.es_info_length, true);

         o << std::endl;

         // output the descriptors
         dumpDescLoop( stream.desc_list, o );
      }
      decOutLevel();
      o << std::endl;
   }
#endif

} // namespace sigen
