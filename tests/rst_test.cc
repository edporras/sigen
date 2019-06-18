#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int rst(TStream& t)
   {
      RST rst;

      for (int i = 0; i < 100; i++) {
         rst.addXportStream(0x1000, 0x2000, 0x100, 0x1000 + i, Dvb::RUNNING_RS);
      }

      DUMP(rst);
      rst.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/rst.ts");
   }
}
