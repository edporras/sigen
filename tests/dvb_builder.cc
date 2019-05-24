//
// usage examples
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include "../src/sigen.h" // this header includes all the required object headers
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   bool write_bin(const TStream& ts, const std::string& basename)
   {
      ts.write(basename + ".ts");
      return true;
   }

   int cmp_bin(const TStream& ts, const std::string& filename)
   {
      std::ifstream inf(filename.c_str(), std::ifstream::binary);
      if (!inf.is_open()) {
         std::cerr << "Error opening file " << filename << " for read" << std::endl;
         return false;
      }

      // determine the size
      inf.seekg (0, std::ios::end);
      size_t size = inf.tellg();

      if (size == 0) {
         std::cerr << "Error: file " << filename << " is empty" << std::endl;
         return false;
      }

      char *blob = new char[size];
      inf.seekg(0, std::ios::beg);
      inf.read(blob, size);
      inf.close();

      // compare
      std::ostringstream os;
      std::for_each(ts.section_list.begin(), ts.section_list.end(),
                    [&](const Section* section) {
                       section->write(os);
                    } );
      int cmp = std::memcmp(os.str().c_str(), blob, size);

      delete [] blob;
      return cmp;
   }
}

//
// sample for building a stream
//
int main(int argc, char* argv[])
{
   enum { NIT_F = 0x001, SDT_F = 0x002, CAT_F = 0x004,
          PAT_F = 0x008, PMT_F = 0x010, BAT_F = 0x020,
          EIT_F = 0x040, TDT_F = 0x100, TOT_F = 0x200 };
   ui16 flags = 0;

   if (argc == 1)
      flags = 0xffff;
   else {
      for (int i = 1; i < argc; i++)
      {
         std::string arg = argv[i];

         if (arg == "-nit")
            flags |= NIT_F;
         else if (arg == "-sdt")
            flags |= SDT_F;
         else if (arg == "-cat")
            flags |= CAT_F;
         else if (arg == "-pat")
            flags |= PAT_F;
         else if (arg == "-pmt")
            flags |= PMT_F;
         else if (arg == "-bat")
            flags |= BAT_F;
         else if (arg == "-eit")
            flags |= EIT_F;
         else if (arg == "-tot")
            flags |= TOT_F;
         else if (arg == "-tdt")
            flags |= TDT_F;
         else if (arg == "-h")
         {
            std::cerr << argv[0] << " linked against sigen library v" << sigen::version() << std::endl;
            return -1;
         }
      }
   }

   // sections are built onto here
   TStream t;

   if (flags & PAT_F) {
      return tests::pat(t);
   }

   if (flags & PMT_F) {
      return tests::pmt(t);
   }

   if (flags & NIT_F) {
      return tests::nit(t);
   }

   if (flags & SDT_F) {
      return tests::sdt(t);
   }

   if (flags & BAT_F) {
      return tests::bat(t);
   }

   if (flags & EIT_F) {
      return tests::eit(t);
   }

   if (flags & TOT_F) {
      return tests::tot(t);
   }

   if (flags & CAT_F) {
      return tests::cat(t);
   }

   if (flags & TDT_F) {
      return tests::tdt(t);
   }

   return 1;
}
