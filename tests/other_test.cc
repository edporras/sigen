#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int other(TStream& t)
   {
      // NIT Actual test
      NITActual nit(0x100, 0x01);

      // ----------------------------------------
      // add network descriptors

      // NetworkNameDesc
      NetworkNameDesc *nnd1 = new NetworkNameDesc("my network"); // allocated with operator new
      nit.addNetworkDesc( *nnd1 ); // always pass by ref
      // don't modify the descriptor once added to the table

      // ClonedDataDesc tests

      // simple 0-byte length
      nit.addNetworkDesc(*new ClonedDataDesc({ 0xa0, 0 }));
      // simple 1-byte
      nit.addNetworkDesc(*new ClonedDataDesc({ 0xa1, 1, 0x23 }));
      // simple n-byte
      nit.addNetworkDesc(*new ClonedDataDesc({ 0xa2, 3, 0x45, 0x67, 0x89 }));

      // error checks
      try {
         // must throw because list only has one byte
         new ClonedDataDesc({0xa3});
         std::cerr << "ClonedDataDesc accepted invalid data (invalid desc data)" << std::endl;
         return 1;
      } catch (std::out_of_range& e) {}

      try {
         // must throw because size byte does not match sequence length
         new ClonedDataDesc({0xa3, 1, 9, 9});
         std::cerr << "ClonedDataDesc accepted invalid data (length mismatch)" << std::endl;
         return 1;
      } catch (std::out_of_range& e) {}

      try {
         // must throw because vector length is > 255
         new ClonedDataDesc(std::vector<ui8>(256, 1));

         std::cerr << "ClonedDataDesc accepted invalid data (length > 255)" << std::endl;
         return 1;
      } catch (std::out_of_range& e) {}

      DUMP(nit);
      nit.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/other.ts");
   }
}
