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
// util.h: miscellaneous utilities
// -----------------------------------

#pragma once

#include <string>
#include "types.h"

namespace sigen {

   // reserved-bit related functions
   // computes a set of bits based on the value of the boolean
   ui32 rbits(bool reserved, ui32 mask);

   // ---------------------------
   // Language Code container class
   //
   class LanguageCode
   {
   private:
      std::string code;

      void refit();

   public:
      enum { BASE_LEN = 3 };

      // constructor
      LanguageCode(const char* c) : code(c) { refit(); }
      LanguageCode(const std::string &c) : code(c) { refit(); }

      // util
      const std::string& str() const { return code; }
      ui32 length() const { return code.length(); }
   };

} // sigen namespace

