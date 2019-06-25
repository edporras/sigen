#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int tot(TStream& t)
   {
      TOT tot(UTC(1, 22, 1999, 10, 0, 0));

      LocalTimeOffsetDesc *ltod = new LocalTimeOffsetDesc;
      UTC t2(1, 22, 1999, 10, 0, 0);
      ltod->addTimeOffset( "eng", 0x22, true, 0x1234, t2, 0x4321 );
      ltod->addTimeOffset( "fre", 0x23, true, 0x5678, t2, 0x8765 );

      tot.addDesc( *ltod );

      DUMP(tot);
      tot.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/tot.ts");
   }
}
