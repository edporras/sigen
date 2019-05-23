#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int tdt(TStream& t)
   {
      // TDT
      TDT tdt(UTC(1, 22, 1999, 10, 0, 0));

      DUMP(tdt);

      tdt.buildSections(t);

      return tests::cmp_bin(t, "reference/tdt.ts");
   }
}
