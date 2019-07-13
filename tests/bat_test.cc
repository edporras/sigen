#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int bat(TStream& t)
   {
      BAT bat(0x1000, 0x10);

      bat.setMaxSectionLen( 300 ); // to test sectionizing

      // ----------------------------------------
      // add bouquet descriptors
      BouquetNameDesc *bnd = new BouquetNameDesc("Bouquet Name");
      bat.addBouquetDesc(*bnd);

      MultilingualBouquetNameDesc* mlbnd = new MultilingualBouquetNameDesc;
      mlbnd->addText("fre", "Jus d'orang");
      mlbnd->addText("spa", "Jugo de naranja");
      bat.addBouquetDesc(*mlbnd);

      CAIdentifierDesc* caid = new CAIdentifierDesc;
      for (int i = 0; i < 30; i++)
         caid->addSystemId( i + 0x3000 );
      bat.addBouquetDesc( *caid );

      // ----------------------------------------
      // add some transport streams
      bat.addXportStream(99, 199);
      bat.addXportStream(98, 198);
      bat.addXportStream(100, 200);
      bat.addXportStream(101, 201);

      // add to the last TS
      StreamIdentifierDesc *sid = new StreamIdentifierDesc(0x02);
      bat.addXportStreamDesc(100, *sid);

      ServiceListDesc *sld1 = new ServiceListDesc;
      for (int i = 0; i < 50; i++)
         sld1->addService( i + 100, 0x01 );

      bat.addXportStreamDesc( *sld1 );

      // add to a TS by id
      ServiceListDesc *sld2 = new ServiceListDesc;
      for (int i = 0; i < 50; i++)
         sld2->addService( i + 200, 0x02 );

      bat.addXportStreamDesc(99, *sld2 );

      DUMP(bat);
      bat.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/bat.ts");
   }
}
