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
// tot.h: class definition for the TOT
// -----------------------------------

#pragma once

#include "utc.h"
#include "table.h"

namespace sigen {

   class TStream;
   class Descriptor;

   /*! \addtogroup table
    *  @{
    */

   /*! \addtogroup DVB
    *  @{
    */

   /*!
    * \brief Time Offset %Table, as per ETSI EN 300 468.
    */
   class TOT : public STable
   {
   public:
      enum {
         PID = 0x14                 //!< Packet PID for transmission.
      };

      /*!
       * \brief Constructor to create table with the current system time.
       */
      TOT() : TOT(UTC()) { }
      /*!
       * \brief Constructor to create table with the specified time.
       * \param time UTC time value.
       */
      TOT(const UTC &time)
         : STable(TID, 7, MAX_SEC_LEN),
         utc(time)
      { }

      // accessors
      virtual ui16 getMaxDataLen() const;
      const UTC& getUTC() const { return utc; }

      /*!
       * \brief Add a Descriptor to the descriptors loop.
       * \param desc Descriptor to add.
       */
      bool addDesc(Descriptor& desc);

      // section data writer
      virtual void buildSections(TStream &) const;

#ifdef ENABLE_DUMP
      void dump(std::ostream &) const;
#endif

   private:
      UTC utc;

      enum { MAX_SEC_LEN = 1024, TID = 0x73 };

      DescList descriptors;
   };
   //! @}
   //! @}
} // namespace
