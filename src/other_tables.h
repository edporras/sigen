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
// other_tables.h: class definitions for the Running Status
//                 and Stuffing tables
// -----------------------------------

#pragma once

#include <list>
#include <string>
#include "table.h"

namespace sigen {

   class TStream;
   class Section;

   /*! \addtogroup table
    *  @{
    */

   /*! \addtogroup DVB
    *  @{
    */

   /*!
    * \brief Running Status %Table, as per ETSI EN 300 468.
    */
   class RST : public STable
   {
   public:
      enum {
         PID = 0x13                            //!< Packet PID for transmission.
      };

      //! \brief Constructor.
      RST() : STable(TID, 0, MAX_SEC_LEN) {}

      /*!
       * \brief Add a transport stream to table.
       * \param xs_id Unique id of the transport stream.
       * \param on_id Id of the originating network.
       * \param sid Id of the service id.
       * \param ev_id Id of the related event.
       * \param running_status Running status of the event. See sigen::Dvb::RunningStatus_t.
       */
      bool addXportStream(ui16 xs_id, ui16 on_id, ui16 sid, ui16 ev_id,
                          ui8  running_status);

      virtual void buildSections(TStream&) const;

#ifdef ENABLE_DUMP
      void dump(std::ostream&) const;
#endif

   private:
      enum { MAX_SEC_LEN = 1024, TID = 0x71 };

      // the private transport stream class
      struct XportStream : public STable::ListItem {
         enum { BASE_LEN = 9 };

         ui16 id;
         ui16 original_network_id;
         ui16 service_id;
         ui16 event_id;
         ui8 running_status : 3;

         // constructor
         XportStream(ui16 xsid, ui16 onid, ui16 sid, ui16 eid, ui8 rs) :
            id(xsid), original_network_id(onid),
            service_id(sid), event_id(eid), running_status(rs) {}
         XportStream() = delete;
      };

      // the list of transport streams
      std::list<XportStream> xport_stream_list;
   };


   /*!
    * \brief %Stuffing %Table, as per ETSI EN 300 468.
    */
   class Stuffing : public STable
   {
   public:
      /*!
       * \brief Constructor using a std::string.
       * \param stuffing_data Stuffing data bytes.
       * \param section_syntax_indicator `true`: for 1, `false` for 0.
       */
      Stuffing(const std::string& stuffing_data, bool section_syntax_indicator = true);
      /*!
       * \brief Constructor specifying a `ui8` to replicate.
       * \param data_length Length of stuffing data to generate.
       * \param data_byte Data byte to repeat.
       * \param section_syntax_indicator `true`: for 1, `false` for 0.
       */
      Stuffing(ui16 data_length, ui8  data_byte, bool section_syntax_indicator = true)
         : Stuffing(std::string(data_length, data_byte), section_syntax_indicator) {}

      // utility
      virtual void buildSections(TStream&) const;

#ifdef ENABLE_DUMP
      void dump(std::ostream&) const;
#endif

   private:
      std::string data;

      enum { TID = 0x72, MAX_SEC_LEN = 4096, CAPACITY = 4093 };
   };
   //! @}
   //! @}
} // namespace
