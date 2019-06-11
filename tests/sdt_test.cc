#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int sdt(TStream& t)
   {
      // SDT
      SDTActual sdt(0x20, 0x30, 0x05);

      sdt.addService(200, true, true, 1, false);

      ServiceDesc *sd = new ServiceDesc( 0xfe,
                                         "My provider name is XYZ and the point of this is to test if really long strings are truncated correctly so I must add even more data here",
                                         "my service name is ABC and the point of this is to test if really really long strings are really really truncated correctly");
      // this addServiceDesc method adds it to the last service added. Otherwise, use addServiceDesc(ui16 sid, Descriptor&)
      sdt.addServiceDesc( *sd );

      CountryAvailabilityDesc *cad = new CountryAvailabilityDesc(true);
      cad->addCountry("eng");
      cad->addCountry("fra");
      cad->addCountry("spa");
      cad->addCountry("ita");
      cad->addCountry("rus");

      sdt.addServiceDesc( *cad );

      StuffingDesc *stuff1 = new StuffingDesc( 'z', 200 );
      sdt.addServiceDesc( *stuff1 );

      std::string s(259, 'd');
      StuffingDesc *stuff2 = new StuffingDesc( s );
      sdt.addServiceDesc( *stuff2 );

      TimeShiftedEventDesc *tsed = new TimeShiftedEventDesc( 0x9999, 0x8888 );
      sdt.addServiceDesc( *tsed );

      LocalTimeOffsetDesc *ltod = new LocalTimeOffsetDesc;
      UTC t2(1, 22, 1999, 10, 0, 0);
      ltod->addTimeOffset( "eng", 0x22, true, 0x1234, t2, 0x4321 );
      ltod->addTimeOffset( "fre", 0x23, true, 0x5678, t2, 0x8765 );

      sdt.addServiceDesc( *ltod );

      TelephoneDesc *td = new TelephoneDesc(true, 0x5, "123-", "1234567-", "123-",
                                            "1234567-", "123456789012345-");
      sdt.addServiceDesc( *td );

      // another service
      sdt.addService(201, false, true, 1, false);

      MultilingualServiceNameDesc *mlsnd = new MultilingualServiceNameDesc;
      mlsnd->addLanguage("fre", "Radio France 1", "Some Service 1");
      mlsnd->addLanguage("spa", "Radio France 2", "Some Service 2");
      mlsnd->addLanguage("eng", "Radio France 3", "Some Service 3");
      mlsnd->addLanguage("deu", "Radio France 4", "Some Service 4");
      mlsnd->addLanguage("ita", "Radio France 5", "Some Service 5");
      mlsnd->addLanguage("rus", "Radio France 6", "Some Service 6");
      mlsnd->addLanguage("chi", "Radio France 7", "Some Service 7");

      sdt.addServiceDesc( *mlsnd );

      NVODReferenceDesc *nrd = new NVODReferenceDesc;
      nrd->addIdentifiers( 0x01, 0x02, 0x03 );

      sdt.addServiceDesc( *nrd );

      DataBroadcastDesc *dbd = new DataBroadcastDesc( 0x3333, 0x2, "test", "eng", "this is the text");
      sdt.addServiceDesc( *dbd );

      DUMP(sdt);
      sdt.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/sdt.ts");
   }
}
