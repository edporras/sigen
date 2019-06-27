#include <cassert>

#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int nit(TStream& t)
   {
      // NIT test
      NITActual nit(0x100, 0x01);

      nit.setMaxSectionLen( 300 ); // to test sectionizing

      // ----------------------------------------
      // add network descriptors

      // NetworkNameDesc
      NetworkNameDesc *nnd1 = new NetworkNameDesc("my network"); // allocated with operator new
      nit.addNetworkDesc( *nnd1 ); // always pass by ref
      // don't modify the descriptor once added to the table

      NetworkNameDesc *nnd2 = new NetworkNameDesc(std::string(259, 'c'));

      // TEST: data was 259 - descriptor should truncate to 253
      assert(nnd2->length() == 257);

      nit.addNetworkDesc( *nnd2 );

      // StuffingDesc
      StuffingDesc *stuff1 = new StuffingDesc( 'z', 13 );

      nit.addNetworkDesc( *stuff1 );

      // second constructor
      std::string data("zzzzzzzzzzzzzzzzzzzzzzz");
      StuffingDesc *stuff2 = new StuffingDesc(data);
      assert(stuff2->length() == (data.length() + 2));

      nit.addNetworkDesc(*stuff2);

      // MultilingualNetworkNameDesc
      MultilingualNetworkNameDesc *mlnnd = new MultilingualNetworkNameDesc;
      mlnnd->addText("fre", "France");
      mlnnd->addText("spa", "Francia");
      mlnnd->addText("eng", "France");
      mlnnd->addText("deu", "Frankreich");

      nit.addNetworkDesc( *mlnnd );

      // ----------------------------------------
      // add some transport streams
      nit.addXportStream(0x10, 0x20); // tsid, onid
      nit.addXportStream(0x11, 0x21);
      nit.addXportStream(0x20, 0x30);
      nit.addXportStream(0x21, 0x31);

      // and descriptors to the last TS added (0x21, 0x31)

      // SatelliteDeliverySystemDesc
      SatelliteDeliverySystemDesc *sdsd = new SatelliteDeliverySystemDesc(1000, 2000, 0x01, 0x08, 0x02);

      nit.addXportStreamDesc( *sdsd );

      // StreamIdentifierDesc
      StreamIdentifierDesc *sid = new StreamIdentifierDesc(0x88);
      nit.addXportStreamDesc( *sid );

      // TimeShiftedServiceDesc
      TimeShiftedServiceDesc *tssd = new TimeShiftedServiceDesc( 0x4000 );
      nit.addNetworkDesc( *tssd );

      // AnnouncementSupportDesc
      AnnouncementSupportDesc *asd;
      asd = new AnnouncementSupportDesc(AnnouncementSupportDesc::EMERGENCY_ALARM_AS |
                                        AnnouncementSupportDesc::WEATHER_FLASH_AS);

      // test adding a ref_type 0x0
      asd->addAnnouncement(AnnouncementSupportDesc::SERVICE_AUDIO_STREAM_RT);

      // test adding a ref_type with service info
      asd->addAnnouncement(AnnouncementSupportDesc::WEATHER_FLASH_AT,
                           AnnouncementSupportDesc::DIFFERENT_SERVICE_RT,
                           0x1100, 0x1300, 0x402, 0x3);

      nit.addXportStreamDesc( *asd );

      // CellFrequencyLinkDesc
      CellFrequencyLinkDesc* cfld = new CellFrequencyLinkDesc;
      cfld->addCell(10, 10000);
      cfld->addCell(10, 11000);
      cfld->addSubCell(10, 11, 1000);
      cfld->addSubCell(10, 12, 3300);
      cfld->addSubCell(12, 3000); // adds to the last cell added
      cfld->addCell(11, 40000);
      cfld->addSubCell(11, 21, 2000);
      nit.addXportStreamDesc(*cfld);

      // CellListDesc
      CellListDesc* cld = new CellListDesc;
      cld->addCell(1, 3000, 2000, 555, 65);
      cld->addSubCell(1, 20, 3000, 2000, 555, 65);
      cld->addSubCell(21, 3001, 2001, 556, 66);
      cld->addCell(2, 4000, 3000, 5555, 655);
      cld->addSubCell(22, 3002, 2002, 557, 668);
      nit.addXportStreamDesc(*cld);

      // add descriptors to a TS by id
      ui16 tsid = 0x20;
      ui16 onid = 0x30;
      CableDeliverySystemDesc *ccd;
      ccd = new CableDeliverySystemDesc(1000, 2000, 0x01, 0x08, 0x02);

      nit.addXportStreamDesc( tsid, onid, *ccd );

      TerrestrialDeliverySystemDesc *tdsd;
      tdsd = new TerrestrialDeliverySystemDesc( 0x1111, 0x44, 0xff,
                                                0x11, 0xff, 0x33,
                                                0xff, 0x22, false);

      nit.addXportStreamDesc( tsid, onid, *tdsd );

      DUMP(nit);
      nit.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/nit.ts");
   }
}
