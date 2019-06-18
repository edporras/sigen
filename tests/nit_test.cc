#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int nit(TStream& t)
   {
      // NIT test
      NITActual nit(0x100, 0x01);
      nit.setMaxSectionLen( 300 );

      // add a network descriptor
      NetworkNameDesc *nnd = new NetworkNameDesc("my network");
      nit.addNetworkDesc( *nnd );

      // another one
      NetworkNameDesc *nd2 = new NetworkNameDesc(std::string(259, 'c')); // should truncate
      nit.addNetworkDesc( *nd2 );

      StuffingDesc *stuff1 = new StuffingDesc( 'z', 13 );
      nit.addNetworkDesc( *stuff1 );

      // multiling net name
      MultilingualNetworkNameDesc *mlnnd = new MultilingualNetworkNameDesc;
      mlnnd->addLanguage("fre", "France");
      mlnnd->addLanguage("spa", "Francia");

      nit.addNetworkDesc( *mlnnd );

      // add a transport stream
      nit.addXportStream(0x20, 0x30);

      // add a descriptor to this transport stream
      CableDeliverySystemDesc *ccd;
      ccd = new CableDeliverySystemDesc(1000, 2000, 0x01, 0x08, 0x02);

      nit.addXportStreamDesc( *ccd );

      TerrestrialDeliverySystemDesc *tdsd;
      tdsd = new TerrestrialDeliverySystemDesc( 0x1111, 0x44, 0xff,
                                                0x11, 0xff, 0x33,
                                                0xff, 0x22, false);

      nit.addXportStreamDesc( *tdsd );

      ServiceListDesc *sld1 = new ServiceListDesc;

      for (int i = 0; i < 50; i++)
         sld1->addService( i + 100, 0x01 );

      nit.addXportStreamDesc( *sld1 );

      // add another transport stream
      nit.addXportStream(0x21, 0x31);

      // and descritpors to it
      SatelliteDeliverySystemDesc *sdsd;
      sdsd = new SatelliteDeliverySystemDesc(1000, 2000, 0x01, 0x08, 0x02);

      nit.addXportStreamDesc( *sdsd );

      StreamIdentifierDesc *sid = new StreamIdentifierDesc(0x88);
      nit.addXportStreamDesc( 0x21, 0x31, *sid ); // add by specifying ts id and onid

      TimeShiftedServiceDesc *tssd = new TimeShiftedServiceDesc( 0x4000 );
      nit.addNetworkDesc( *tssd );

      ServiceListDesc *sld2 = new ServiceListDesc;
      for (int i = 0; i < 50; i++)
         sld2->addService( i + 200, 0x02 );

      nit.addXportStreamDesc( *sld2 );

      AnnouncementSupportDesc *asd;
      asd = new AnnouncementSupportDesc(AnnouncementSupportDesc::EMERGENCY_ALARM_AS |
                                        AnnouncementSupportDesc::WEATHER_FLASH_AS);
      asd->addAnnouncement( AnnouncementSupportDesc::EMERGENCY_ALARM,
                            AnnouncementSupportDesc::SERVICE_AUDIO_STREAM,
                            0x1000, 0x1200, 0x400, 0x3 );
      asd->addAnnouncement( AnnouncementSupportDesc::WEATHER_FLASH,
                            AnnouncementSupportDesc::DIFFERENT_SERVICE,
                            0x1100, 0x1300, 0x402, 0x3 );

      nit.addXportStreamDesc( *asd );

      CellFrequencyLinkDesc* cfld = new CellFrequencyLinkDesc;
      cfld->addLink(10, 10000);
      cfld->addLink(10, 11000);
      cfld->addLinkSubCell(10, 10000, 11, 1000);
      cfld->addLinkSubCell(10, 10000, 12, 3300);
      cfld->addLinkSubCell(12, 3000); // adds to the last Link added
      cfld->addLink(11, 40000);
      cfld->addLinkSubCell(11, 40000, 21, 2000);
      nit.addXportStreamDesc(*cfld);

      CellListDesc* cld = new CellListDesc;
      cld->addCell(1, 3000, 2000, 555, 65);
      cld->addCellSubCell(1, 20, 3000, 2000, 555, 65);
      cld->addCellSubCell(21, 3001, 2001, 556, 66);
      cld->addCell(2, 4000, 3000, 5555, 655);
      cld->addCellSubCell(22, 3002, 2002, 557, 668);
      nit.addXportStreamDesc(*cld);

      DUMP(nit);
      nit.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/nit.ts");
   }
}
