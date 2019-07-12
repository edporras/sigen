#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int eit(TStream& t)
   {
      // EIT PF Actual
      PF_EITActual eit_a(100, 0x333, 0x444, 0);

      eit_a.setMaxSectionLen( 300 ); // to test sectionizing

      // ----------------------------------------
      // add a present event with descriptors
      eit_a.addPresentEvent(0x1000, UTC(3, 1, 1999, 9, 0, 0), BCDTime(0, 30, 0), 1, 1);

      PDCDesc* pdcd = new PDCDesc(0x1000);
      eit_a.addPresentEventDesc(*pdcd);

      PrivateDataSpecifierDesc* pdsd = new PrivateDataSpecifierDesc( 0x44446666 );
      eit_a.addPresentEventDesc( *pdsd );

      MultilingualComponentDesc* mlcd = new MultilingualComponentDesc(0x22);
      mlcd->addText("fre", "Je sui fatigue");
      mlcd->addText("spa", "Estoy cansado");
      eit_a.addPresentEventDesc(*mlcd);

      // ----------------------------------------
      // add a following event with descriptors
      eit_a.addFollowingEvent(0x1001, UTC(3, 1, 1999, 9, 30, 0), BCDTime(0, 30, 0), 1, 1);

      DSNGDesc* dsngd = new DSNGDesc("dsng data");
      eit_a.addFollowingEventDesc(0x1001, *dsngd);

      PartialTransportStreamDesc* ptsd = new PartialTransportStreamDesc(0x3000, 0x50, 0x1000);
      eit_a.addFollowingEventDesc(0x1001, *ptsd);

      TransportStreamDesc* tsd = new TransportStreamDesc;
      eit_a.addFollowingEventDesc(0x1001, *tsd);

      // ContentDesc
      ContentDesc *cond = new ContentDesc;
      cond->addContent( 0x1, 0x1, 0xc, 0x1 );
      cond->addContent( 0x2, 0x1, 0xb, 0x1 );
      cond->addContent( 0x3, 0x1, 0xa, 0x1 );
      cond->addContent( 0x4, 0x1, 0x9, 0x1 );
      cond->addContent( 0x5, 0x1, 0x8, 0x1 );
      cond->addContent( 0x6, 0x1, 0x7, 0x1 );

      eit_a.addFollowingEventDesc( *cond );

      // ShortSmoothingBufferDesc
      ShortSmoothingBufferDesc *smbd = new ShortSmoothingBufferDesc( 0x1, 0x4, "xxxxxxxxx");
      eit_a.addFollowingEventDesc( *smbd );

      // ParentalRatingDesc
      ParentalRatingDesc *prd = new ParentalRatingDesc;
      prd->addRating( "eng", 0x01 );
      prd->addRating( "fre", 0x02 );
      prd->addRating( "deu", 0x01 );
      prd->addRating( "ita", 0x02 );
      prd->addRating( "gre", 0x03 );
      prd->addRating( "spa", 0x04 );

      eit_a.addFollowingEventDesc( *prd );

      DUMP(eit_a);
      eit_a.buildSections(t);

      // EIT PDF Other
      PF_EITOther eit_o(101, 0x444, 0x555, 0);
      eit_o.addPresentEvent(0x2000, UTC(3, 1, 1999, 9, 0, 0), BCDTime(0, 30, 0), 1, 1);

      ShortEventDesc *sed = new ShortEventDesc( "eng", "Fight Club", "An insomniac office worker looking for a way to change his life crosses paths with a devil-may-care soap maker.");
      eit_o.addPresentEventDesc( *sed );

      DUMP(eit_o);
      eit_o.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/eit.ts");
   }
}
