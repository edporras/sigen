#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int bat(TStream& t)
   {
      BAT bat(0x1000, 0x10);

      bat.addXportStream(100, 200);
      bat.addXportStream(100, 201);

      BouquetNameDesc *bnd = new BouquetNameDesc("Bouquet Name");
      bat.addBouquetDesc(*bnd);

      StreamIdentifierDesc *sid = new StreamIdentifierDesc(0x02);
      bat.addXportStreamDesc(100, 201, *sid);

      DUMP(bat);
      bat.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/bat.ts");
   }
}
