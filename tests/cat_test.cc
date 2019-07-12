#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int cat(TStream& t)
   {
      // cat table
      CAT cat(0x14);

      CADesc *CAd = new CADesc( 0x4653, 0x1234, "this is a test" );
      cat.addDesc( *CAd );

      DUMP(cat);
      cat.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/cat.ts");
   }
}
