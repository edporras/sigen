#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int eit(TStream& t)
   {
      // TODO: this needs more descriptors, events, etc.
      PF_EIT eit(100, 0x333, 0x444, PF_EIT::ACTUAL, 0);

      DUMP(eit);
      eit.buildSections(t);

      return tests::cmp_bin(t, "reference/eit.ts");
   }
}
