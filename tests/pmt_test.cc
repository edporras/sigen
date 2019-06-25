#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int pmt(TStream& t)
   {
      // build the pmt
      PMT pmt( 100, 101, 0x01 );

      // create some descriptors
      SubtitlingDesc *subtd = new SubtitlingDesc;
      subtd->addSubtitling( "eng", 0x01, 0x1000, 0x1010 );
      subtd->addSubtitling( "fre", 0x01, 0x1000, 0x1010 );
      subtd->addSubtitling( "deu", 0x01, 0x1000, 0x1010 );
      subtd->addSubtitling( "spa", 0x01, 0x1000, 0x1010 );
      subtd->addSubtitling( "rus", 0x01, 0x1000, 0x1010 );

      // once added, the table claims ownership of the pointer - so do
      // not free it. Also, no further modifications to the descriptor
      // should be done after this.
      pmt.addProgramDesc( *subtd );


      TeletextDesc *ttd = new TeletextDesc;
      ttd->addTeletext( "eng", 0x01, 0x01, 0x11 );
      ttd->addTeletext( "fre", 0x01, 0x01, 0x11 );
      ttd->addTeletext( "deu", 0x01, 0x01, 0x11 );
      ttd->addTeletext( "spa", 0x01, 0x01, 0x11 );
      ttd->addTeletext( "ita", 0x01, 0x01, 0x11 );
      ttd->addTeletext( "chi", 0x01, 0x01, 0x11 );
      ttd->addTeletext( "rus", 0x01, 0x01, 0x11 );

      pmt.addProgramDesc( *ttd );

      ISO639LanguageDesc *langd = new ISO639LanguageDesc;
      langd->addLanguage( "eng", 0x02 );
      langd->addLanguage( "fre", 0x03 );
      langd->addLanguage( "deu", 0x04 );
      langd->addLanguage( "ita", 0x05 );
      langd->addLanguage( "spa", 0x06 );

      pmt.addProgramDesc( *langd );

      AC3Desc *ac3d = new AC3Desc;
      ac3d->setASVC(0x4);
      ac3d->setBSID(0x2);

      pmt.addProgramDesc( *ac3d );

      AncillaryDataDesc *add = new AncillaryDataDesc(0x51);
      pmt.addProgramDesc( *add );

      // add streams
      pmt.addElemStream( sigen::PMT::ES_ISO_IEC_11172_AUDIO, 0x21 );
      pmt.addElemStream( sigen::PMT::ES_ISO_IEC_11172_VIDEO, 0x22 );

      // and descriptors to it. As above, the table will claim
      // ownership and handle freeing it
      LinkageDesc *ld = new LinkageDesc( 0x001, 0x002, 0x003, 0xf );
      pmt.addElemStreamDesc( *ld );

      MobileHandoverLinkageDesc *mhld = new MobileHandoverLinkageDesc(0x2000, 0x1000, 100,
                                                                      MobileHandoverLinkageDesc::HO_ASSOCIATED_SERVICE,
                                                                      100, 20);
      pmt.addElemStreamDesc( 0x21, *mhld ); // add by matching es pid

      try
      {
         // should throw
         LinkageDesc *ld = new LinkageDesc( 0x001, 0x002, 0x003, LinkageDesc::SSUS );
      } catch (std::domain_error& e) {}

      SSUDataBroadcastIdDesc* ssudbid = new SSUDataBroadcastIdDesc;
      std::vector<ui8> v = { 1, 2, 3, 4, 5, 6, 7 };
      ssudbid->addOUI(0x2001,
                      SSUDataBroadcastIdDesc::UPDATE_TYPE_SSU_USING_RETURN_CHANNEL,
                      2, v);
      pmt.addElemStreamDesc(*ssudbid);

      SSUScanLinkageDesc* ssusld = new SSUScanLinkageDesc(0x1000, 0x2000, 0x100, SSUScanLinkageDesc::TABLE_TYPE_BAT);
      pmt.addElemStreamDesc(*ssusld);

      SSULinkageDesc* ssuld = new SSULinkageDesc(0x1000, 0x2000, 0x101);
      ssuld->addOUI(0x12345, v);
      pmt.addElemStreamDesc(*ssuld);

      DUMP(pmt);
      pmt.buildSections(t);

      // dump built sections
      DUMP(t);

      return cmp_bin(t, "reference/pmt.ts");
   }
}
