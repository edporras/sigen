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
// nit_bar.h: class definition for the NIT & BAT
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "table.h"

namespace sigen {

   class Descriptor;

   /*! \addtogroup abstract
    *  @{
    */

   /*!
    * \brief Abstract class for the NIT & BAT classes.
    *
    * The NIT & BAT are basically exactly the same. THis base class
    * handles all the logic but can't be instantiaited. Instead, use
    * NITActual, NITOther, and BAT classes below.
    */
   class NIT_BAT : public PSITable
   {
   public:
      enum { NIT_ACTUAL_TID = 0x40, NIT_OTHER_TID = 0x41, BAT_TID = 0x4a };

      // add a Descriptor to the class. Aliased in the NIT and BAT
      // classes as addNetworkDesc() and addBouquetDesc()
      // respectively.
      bool addDesc(Descriptor &);

      /*!
       * \brief Add a transport stream to table.
       * \param xs_id Unique id of the transport stream.
       * \param on_id Id of the originating network.
       */
      bool addXportStream(ui16 xs_id, ui16 on_id);
      /*!
       * \brief Add a Descriptor to last added transport stream.
       * \param desc Descriptor to add.
       */
      bool addXportStreamDesc(Descriptor& desc);
      /*!
       * \brief Add a Descriptor to the transport stream specified.
       * \param xs_id Id of the transport stream.
       * \param on_id Id of the originating network.
       * \param desc Descriptor to add.
       */
      bool addXportStreamDesc(ui16 xs_id, ui16 on_id, Descriptor& desc);

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   private:
      enum { MAX_SEC_LEN = 1024 };

      // the transport stream struct - public as the BAT uses it too
      struct XportStream : public PSITable::ListItem
      {
         enum { BASE_LEN = 6 };

         ui16 id;                  // transport stream id
         ui16 original_network_id;

         // constructor
         XportStream(ui16 tsid, ui16 onid) :
            id(tsid), original_network_id(onid) { }
         XportStream() = delete;

         bool equals(ui16 tsid, ui16 onid) const { return (tsid == id && onid == original_network_id); }

         // writes item header bytes, returns num bytes written
         virtual ui8 write_header(Section& sec) const;
      };

      // NIT members
      ui16 xport_stream_loop_length;
      DescList descriptors;
      std::list<XportStream> xport_streams;

      // private methods
      bool addXportStreamDesc(XportStream& , Descriptor &);
      virtual bool writeSection(Section& , ui8, ui16 &) const;

#ifdef ENABLE_DUMP
      void dumpXportStreams(std::ostream &) const;
#endif

      // section building state tracking
      enum State_t { INIT, WRITE_HEAD, GET_NET_DESC, WRITE_NET_DESC, WRITE_XPORT_LOOP_LEN,
                     GET_XPORT_STREAM, WRITE_XPORT_STREAM };
      mutable struct Context {
         Context() :
            nd_done(false), op_state(INIT), nd(nullptr), ts(nullptr)
         {}

         bool nd_done;
         State_t op_state;

         const Descriptor *nd;
         const XportStream *ts;
         std::list<std::unique_ptr<Descriptor> >::const_iterator nd_iter;
         std::list<XportStream>::const_iterator ts_iter;
      } run;

   protected:
      // protected constructor - type refers to ACTUAL or OTHER,
      // reserved is the state of reserved bits
      NIT_BAT(ui8 type, ui16 id, ui8 ver, bool cni = true) :
         PSITable(type, id, 9, MAX_SEC_LEN, ver, cni),
         xport_stream_loop_length(0)
      { }
   };

   /*!
    * \brief Abstract Network Information %Table base class.
    */
   class NIT : public NIT_BAT {
   public:
      enum {
         PID = 0x10                           //!< Packet PID for transmission.
      };

      /*!
       * \brief Add a Descriptor to the Network Descriptors loop.
       * \param desc Descriptor to add.
       */
      bool addNetworkDesc(Descriptor& desc) { return addDesc(desc); }

   protected:
      // protected constructor - type refers to ACTUAL or OTHER,
      // reserved is the state of reserved bits
      NIT(ui8 type, ui16 network_id, ui8 ver, bool cni) :
         NIT_BAT(type, network_id, ver, cni)
      { }
   };
   //! @}

   /*! \addtogroup table
    *  @{
    */

   /*!
    * \brief Network Information %Table - Actual, as per ETSI EN 300 468.
    */
   struct NITActual : public NIT
   {
      /*!
       * \brief Constructor.
       * \param network_id Id identifying the network.
       * \param version_number Version number to use the subtable.
       * \param current_next_indicator `true`: version curently applicable, `false`: next applicable.
       */
      NITActual(ui16 network_id, ui8 version_number, bool current_next_indicator = true)
         : NIT(NIT_BAT::NIT_ACTUAL_TID, network_id, version_number, current_next_indicator)
      { }
   };

   /*!
    * \brief Network Information %Table - Other, as per ETSI EN 300 468
    */
   struct NITOther : public NIT
   {
      /*!
       * \brief Constructor.
       * \param network_id Id identifying the network.
       * \param version_number Version number to use the subtable.
       * \param current_next_indicator `true`: version curently applicable, `false`: next applicable.
       */
      NITOther(ui16 network_id, ui8 version_number, bool current_next_indicator = true)
         : NIT(NIT_BAT::NIT_OTHER_TID, network_id, version_number, current_next_indicator)
      { }
   };

   /*!
    * \brief Bouquet Association %Table, as per ETSI EN 300 468.
    */
   struct BAT : public NIT_BAT
   {
      enum {
         PID = 0x11                          //!< Packet PID for transmission.
      };

      /*!
       * \brief Constructor.
       * \param bouquet_id Id identifying the bouquet.
       * \param version_number Version number to use the subtable.
       * \param current_next_indicator `true`: version curently applicable, `false`: next applicable.
       */
      BAT(ui16 bouquet_id, ui8  version_number, bool current_next_indicator = true)
         : NIT_BAT(NIT_BAT::BAT_TID, bouquet_id, version_number, current_next_indicator)
      { }

      /*!
       * \brief Add a Descriptor to the Bouquet Descriptors loop.
       * \param desc Descriptor to add.
       */
      bool addBouquetDesc(Descriptor& desc) { return addDesc(desc); }
   };

   //! @}

} // sigen namespace
