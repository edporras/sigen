#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int cat(TStream& t)
   {
      // cat table
      CAT cat(0x14);

      AudioStreamDesc *asd = new AudioStreamDesc(true, false, 0x22);
      cat.addDesc( *asd );

      VideoStreamDesc *vsd1 = new VideoStreamDesc( false, 0x09, false, true );
      cat.addDesc( *vsd1 );

      VideoStreamDesc *vsd2 = new VideoStreamDesc( false, 0x09, false, true,
                                                   0x88, 0x2, true );
      cat.addDesc( *vsd2 );

      CADesc *CAd = new CADesc( 0x4653, 0x1234, "this is a test" );
      cat.addDesc( *CAd );

      HierarchyDesc *hd = new HierarchyDesc( 0x02, 0x15, 0x16, 0x17 );
      cat.addDesc( *hd );

      MaximumBitrateDesc *mbd = new MaximumBitrateDesc( 0x234243 );
      cat.addDesc( *mbd );

      CopyrightDesc *crd = new CopyrightDesc( 0x88990011, "(c) 2001 ep");
      cat.addDesc( *crd );

      MultiplexBufferUtilizationDesc *mbud = new MultiplexBufferUtilizationDesc( false, 0xefff, 0x27 );
      cat.addDesc( *mbud );

      SystemClockDesc *scd = new SystemClockDesc( true, 0x7, 0x3);
      cat.addDesc( *scd );

      TargetBackgroundGridDesc *tbgd = new TargetBackgroundGridDesc( 0x0001, 0x0002, 0x1);
      cat.addDesc( *tbgd );

      VideoWindowDesc *vwd = new VideoWindowDesc( 0x0002, 0x0001, 0x03 );
      cat.addDesc( *vwd );

      RegistrationDesc *rd = new RegistrationDesc( 0x43523412, "" );
      cat.addDesc( *rd );

      STDDesc *stdd = new STDDesc( false );
      cat.addDesc( *stdd );

      SmoothingBufferDesc *sbd = new SmoothingBufferDesc( 0x334455, 0x445566 );
      cat.addDesc( *sbd );

      IBPDesc *id = new IBPDesc( true, false, 0x1111);
      cat.addDesc( *id );

      ContentDesc *cond = new ContentDesc;
      cond->addContent( 0x1, 0x1, 0xc, 0x1 );
      cond->addContent( 0x2, 0x1, 0xb, 0x1 );
      cond->addContent( 0x3, 0x1, 0xa, 0x1 );
      cond->addContent( 0x4, 0x1, 0x9, 0x1 );
      cond->addContent( 0x5, 0x1, 0x8, 0x1 );
      cond->addContent( 0x6, 0x1, 0x7, 0x1 );

      cat.addDesc( *cond );

      FrequencyListDesc *fld = new FrequencyListDesc( 0x2 );
      fld->addFrequency( 0x1000 );
      fld->addFrequency( 0x2000 );
      fld->addFrequency( 0x3000 );
      fld->addFrequency( 0x4000 );
      fld->addFrequency( 0x5000 );
      fld->addFrequency( 0x6000 );

      cat.addDesc( *fld );

      ParentalRatingDesc *prd = new ParentalRatingDesc;
      prd->addRating( "eng", 0x01 );
      prd->addRating( "fre", 0x02 );
      prd->addRating( "deu", 0x01 );
      prd->addRating( "ita", 0x02 );
      prd->addRating( "gre", 0x03 );
      prd->addRating( "spa", 0x04 );

      cat.addDesc( *prd );

      ShortEventDesc *sed = new ShortEventDesc( "eng", "Fight Club", "An insomniac office worker looking for a way to change his life crosses paths with a devil-may-care soap maker.");
      cat.addDesc( *sed );

      ShortSmoothingBufferDesc *smbd = new ShortSmoothingBufferDesc( 0x1, 0x4, "What is a smoothing buffer?");
      cat.addDesc( *smbd );

      ComponentDesc *compd = new ComponentDesc( 0x2, 0x4, 0x5, "eng", "blah blah blah blah" );
      cat.addDesc( *compd );

      DataBroadcastIdDesc *dbid = new DataBroadcastIdDesc(0x4545);

      cat.addDesc( *dbid );

      DUMP(cat);
      cat.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/cat.ts");
   }
}
