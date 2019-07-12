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
// sdt.h: class definition for the SDT
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
    * \brief Abstract Service Description %Table base class.
    */
   class SDT : public PSITable
   {
   public:
      enum {
         PID = 0x11                                    //!< Packet PID for transmission.
      };
      enum { ACTUAL_TID = 0x42, OTHER_TID = 0x46 };

      /*!
       * \brief Add a service to the table.
       * \param service_id Unique id of service within the TS.
       * \param eit_schedule_flag EIT-schedule information present in current TS.
       * \param eit_present_following_flag EIT-PF present in current TS.
       * \param running_status Running status of the service as per sigen::Dvb::RunningStatus_t.
       * \param free_CA_mode `false`: no service components are scrambled; `true`: one ore more controlled by CA system.
       */
      bool addService(ui16 service_id, bool eit_schedule_flag, bool eit_present_following_flag,
                      ui8  running_status, bool free_CA_mode);
      /*!
       * \brief Add a Descriptor to the most recently added service.
       * \param desc Descriptor to add.
       */
      bool addServiceDesc(Descriptor& desc);
      /*!
       * \brief Add a Descriptor to the transport stream specfied.
       * \param service_id Id of service to add descriptor to.
       * \param desc Descriptor to add.
       */
      bool addServiceDesc(ui16 service_id, Descriptor& desc);

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   private:
      enum { MAX_SEC_LEN = 1024 };

      // the service holder class - private to the sdt
      struct Service : public PSITable::ListItem {
         enum { BASE_LEN = 5 };

         ui16 id;
         ui8 running_status : 3;
         bool eit_schedule;
         bool eit_present_following;
         bool free_ca_mode;

         // constructor
         Service(ui16 sid, bool esf, bool epff, ui8 rs, bool fca) :
            id(sid), running_status(rs),
            eit_schedule(esf), eit_present_following(epff),
            free_ca_mode(fca) {}
         Service() = delete;

         bool equals(ui16 sid) const { return sid == id; }

         // writes item header bytes, returns num bytes written
         virtual ui8 write_header(Section& sec) const;
         // writes the 2-byte desc loop len
         virtual void write_desc_loop_len(Section& sec, ui8* pos, ui16 len) const;
      };

      // sdt data members begin here
      ui16 original_network_id;

      // the list of services
      std::list<Service> service_list;

      enum State_t { INIT, WRITE_HEAD, GET_SERVICE, WRITE_SERVICE };
      mutable struct Context {
         Context() : op_state(INIT), serv(nullptr) {}
         
         State_t op_state;
         const Service *serv;
         std::list<Service>::const_iterator s_iter;
      } run;

   protected:
      // constructor
      SDT(ui8 type, ui16 xport_str_id, ui16 orig_network_id, ui8 ver,
          bool cni = true) :
         PSITable(type, xport_str_id, 8, MAX_SEC_LEN, ver, cni),
         original_network_id(orig_network_id)
      { }

      bool addServiceDesc(Service&, Descriptor &);
      virtual bool writeSection(Section&, ui8, ui16 &) const;
   };
   //! @}

   /*! \addtogroup table
    *  @{
    */

   /*! \addtogroup DVB
    *  @{
    */

   /*!
    * \brief Service Description %Table - Actual, as per ETSI EN 300 468.
    */
   struct SDTActual : public SDT {
      /*!
       * \brief Constructor.
       * \param xs_id Id identifying the transport stream.
       * \param on_id Id identifying the originating network.
       * \param version_number Version number to use the subtable.
       * \param current_next_indicator `true`: version curently applicable, `false`: next applicable.
       */
      SDTActual(ui16 xs_id, ui16 on_id, ui8 version_number, bool current_next_indicator = true)
         : SDT(SDT::ACTUAL_TID, xs_id, on_id, version_number, current_next_indicator) { }
   };

   /*!
    * \brief Service Description %Table - Actual, as per ETSI EN 300 468.
    */
   struct SDTOther : public SDT {
      /*!
       * \brief Constructor.
       * \param xs_id Id identifying the transport stream.
       * \param on_id Id identifying the originating network.
       * \param version_number Version number to use the subtable.
       * \param current_next_indicator `true`: version curently applicable, `false`: next applicable.
       */
      SDTOther(ui16 xs_id, ui16 on_id, ui8 version_number, bool current_next_indicator = true)
         : SDT(SDT::OTHER_TID, xs_id, on_id, version_number, current_next_indicator) { }
   };
   //! @}
   //! @}
}
