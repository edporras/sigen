#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int tot(TStream& t)
   {
      TOT tot(UTC(1, 22, 1999, 10, 0, 0));

      CAIdentifierDesc *caid = new CAIdentifierDesc;
      for (int i = 0; i < 30; i++)
         caid->addSystemId( i + 0x3000 );
      tot.addDesc( *caid );

      PrivateDataSpecifierDesc *pdsd = new PrivateDataSpecifierDesc( 0x44446666 );
      tot.addDesc( *pdsd );

      PrivateDataIndicatorDesc *pdid = new PrivateDataIndicatorDesc( 0x55553333 );
      tot.addDesc( *pdid );

      DataStreamAlignmentDesc *dsad = new DataStreamAlignmentDesc( 0xff );
      tot.addDesc( *dsad );

      BouquetNameDesc * bnd = new BouquetNameDesc("MY BOUQUET");
      tot.addDesc( *bnd );

      ServiceMoveDesc *smd = new ServiceMoveDesc(0x01, 0x02, 0x03);
      tot.addDesc( *smd );

      MultilingualBouquetNameDesc *mlbnd = new MultilingualBouquetNameDesc;
      mlbnd->addLanguage("fre", "Jus d'orang");
      mlbnd->addLanguage("spa", "Jugo de naranja");

      tot.addDesc(*mlbnd);

      MultilingualComponentDesc *mlcd = new MultilingualComponentDesc(0x22);
      mlcd->addLanguage("fre", "Je sui fatigue");
      mlcd->addLanguage("spa", "Estoy cansado");

      tot.addDesc(*mlcd);

      DUMP(tot);
      tot.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/tot.ts");
   }
}
