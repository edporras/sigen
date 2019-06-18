#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int st(TStream& t)
   {
      // pass string
      std::string data(600, 0xaa);
      Stuffing st1(data);
      DUMP(st1);
      st1.buildSections(t);

      // use direct constructor - note that as of v2.7.1, args are
      // swapped to match std::string constructor above
      Stuffing st2(5000, 0xbb); // should be truncated to 4093 data length
      DUMP(st2);
      st2.buildSections(t);

      // stdout
      DUMP(t);

      return tests::cmp_bin(t, "reference/st.ts");
   }
}
