//
// usage examples
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <map>
#include <stdexcept>
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
         std::stringstream s;
         s << "Error opening file " << filename << " for read";
         throw std::runtime_error(s.str());
      }

      // determine the size
      inf.seekg (0, std::ios::end);
      size_t size = inf.tellg();
      if (size == 0) {
         std::stringstream s;
         s << "Error: file " << filename << " is empty";
         throw std::runtime_error(s.str());
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


void usage(const std::string& prog)
{
   std::cerr << prog << " linked against sigen library v" << sigen::version() << std::endl
             << "Usage: " << prog << " [-bat|-cat|-eit|-nit|-pat|-pmt|-sdt|-tdt|-tot]"
             << std::endl;
}

//
// sample for building a stream
//
int main(int argc, char* argv[])
{
   // expecting one arg
   if ((argc != 2) ||
       (std::string(argv[1]) == "-h")) {
      usage(argv[0]);
      return 1;
   }

   // build a lookup table of what to run
   typedef int (*test_fn)(sigen::TStream&);
   const std::map<std::string, test_fn> opts = {
      { "-bat", tests::bat },
      { "-cat", tests::cat },
      { "-eit", tests::eit },
      { "-nit", tests::nit },
      { "-pat", tests::pat },
      { "-pmt", tests::pmt },
      { "-sdt", tests::sdt },
      { "-tdt", tests::tdt },
      { "-tot", tests::tot }
   };

   // search for the given argument
   auto it = opts.find(argv[1]);
   if (it == opts.end()) {
      usage(argv[0]);
      return 1;
   }

   // build a tstream to pass to it and run the test so the table
   // builds its sections onto it
   TStream t;
   return it->second(t);
}
