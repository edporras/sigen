// Copyright 1999-2020 Ed Porras
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
// util_desc.h
// -----------------------------------

#pragma once

#include "descriptor.h"

namespace sigen {
   /*! \addtogroup utility
    *  @{
    */

   /*!
    * \brief Utility pseudo-descriptor that doesn't generate section
    * data. To be used in the NIT/BAT's network descriptors loop.
    */
   class UtilityDesc : public Descriptor
   {
   public:

      /*!
       * \enum  Type
       *
       * \brief Identifier for the type of descriptor to instantiate.
       */
      enum Type {
         SECTION_END_MARK = 1   //!< Section-End marker. Force the current section to end.
      };

      /*!
       * \brief Constructor.
       */
      UtilityDesc(Type t) : Descriptor(0), u_type(t) {}

      virtual ui32 type() const { return u_type; }

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &o) const;
#endif

   private:
      Type u_type;

      virtual void buildSections(Section&) {}
      virtual ui16 length() const { return 0; }
   };

   //! @}
} // sigen namespace
