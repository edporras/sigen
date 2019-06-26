#include <cassert>
#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int pmt(TStream& t)
   {
      // build the pmt
      PMT pmt( 100, 101, 0x01 );

      pmt.setMaxSectionLen( 200 ); // to test sectionizing

      // Subtitling Desc
      SubtitlingDesc *subtd = new SubtitlingDesc;
      subtd->addSubtitling( "eng", 0x01, 0x1000, 0x1010 );
      subtd->addSubtitling( "fre", 0x01, 0x1001, 0x1010 );
      subtd->addSubtitling( "deu", 0x01, 0x1002, 0x1010 );
      subtd->addSubtitling( "spa", 0x01, 0x1003, 0x1010 );
      subtd->addSubtitling( "rus", 0x01, 0x1004, 0x1010 );

      // once added, the table claims ownership of the pointer - so do
      // not free it. Also, no further modifications to the descriptor
      // should be done after this.
      pmt.addProgramDesc( *subtd );

      // Teletext Descriptor
      TeletextDesc *ttd = new TeletextDesc;
      ttd->addTeletext( "eng", 0x01, 0x01, 0x11 );
      ttd->addTeletext( "fre", 0x01, 0x02, 0x11 );
      ttd->addTeletext( "deu", 0x01, 0x03, 0x11 );
      ttd->addTeletext( "spa", 0x01, 0x04, 0x11 );
      ttd->addTeletext( "ita", 0x01, 0x05, 0x11 );
      ttd->addTeletext( "chi", 0x01, 0x06, 0x11 );
      ttd->addTeletext( "rus", 0x01, 0x07, 0x11 );

      pmt.addProgramDesc( *ttd );

      // ISO639 Language Descriptor
      ISO639LanguageDesc *langd = new ISO639LanguageDesc;
      langd->addLanguage( "eng", 0x02 );
      langd->addLanguage( "fre", 0x03 );
      langd->addLanguage( "deu", 0x04 );
      langd->addLanguage( "ita", 0x05 );
      langd->addLanguage( "spa", 0x06 );

      pmt.addProgramDesc( *langd );

      // AC3 Descriptor
      AC3Desc *ac3d = new AC3Desc;
      ac3d->setASVC(0x4);
      ac3d->setBSID(0x2);

      pmt.addProgramDesc( *ac3d );

      // Extended AC3 Descriptor
      ExtendedAC3Desc *eac3d = new ExtendedAC3Desc;
      eac3d->setComponentType(0x01);
      eac3d->setMixinfoExists();
      eac3d->setSubstream1(0x22);

      pmt.addProgramDesc(*eac3d);

      // Ancillary Data Descriptor
      AncillaryDataDesc *add = new AncillaryDataDesc(0x51);
      pmt.addProgramDesc( *add );

      // Private Data Indicator
      PrivateDataIndicatorDesc* pdid = new PrivateDataIndicatorDesc( 0x55553333 );
      pmt.addProgramDesc( *pdid );

      // Service Move
      ServiceMoveDesc* smd = new ServiceMoveDesc(0x01, 0x02, 0x03);
      pmt.addProgramDesc( *smd );

      // add streams
      pmt.addElemStream( sigen::PMT::ES_ISO_IEC_11172_AUDIO, 0x21 );
      pmt.addElemStream( sigen::PMT::ES_ISO_IEC_11172_VIDEO, 0x22 );

      // and descriptors to it. As above, the table will claim
      // ownership and handle freeing it
      // DataSTreamAlignmentDesc
      DataStreamAlignmentDesc* dsad = new DataStreamAlignmentDesc( 0xff );
      pmt.addElemStreamDesc( *dsad );

      // MobileHandoverLinkageDesc
      MobileHandoverLinkageDesc *mhld = new MobileHandoverLinkageDesc(0x2000, 0x1000, 100,
                                                                      MobileHandoverLinkageDesc::HO_ASSOCIATED_SERVICE,
                                                                      100, 20);
      pmt.addElemStreamDesc( 0x21, *mhld ); // add by matching es pid

      // SSUDataBroadcastIdDesc
      SSUDataBroadcastIdDesc* ssudbid = new SSUDataBroadcastIdDesc;
      std::vector<ui8> v = { 1, 2, 3, 4, 5, 6, 7 };
      ssudbid->addOUI(0x2001,
                      SSUDataBroadcastIdDesc::UPDATE_TYPE_SSU_USING_RETURN_CHANNEL,
                      2, v);
      pmt.addElemStreamDesc(*ssudbid);

      // LinkageDesc
      LinkageDesc *ld = new LinkageDesc( 0x001, 0x002, 0x003, 0xf );
      pmt.addElemStreamDesc( *ld );

      try
      {
         // throws because SSULinkageDesc should be used instead
         LinkageDesc *ld = new LinkageDesc( 0x001, 0x002, 0x003, LinkageDesc::SSUS );
         // so this should not happen
         assert(true);
      } catch (std::domain_error& e) {}

      SSULinkageDesc* ssuld = new SSULinkageDesc(0x1000, 0x2000, 0x101);
      ssuld->addOUI(0x12345, v);
      pmt.addElemStreamDesc(*ssuld);

      try
      {
         // throws because SSUScanLinkageDesc should be used instead
         LinkageDesc *ld = new LinkageDesc( 0x001, 0x002, 0x003, LinkageDesc::TS_SSU_BAT_OR_NIT);
         // so this should not happen
         assert(true);
      } catch (std::domain_error& e) {}

      // SSUScanLinkageDesc
      SSUScanLinkageDesc* ssusld = new SSUScanLinkageDesc(0x1000, 0x2000, 0x100, SSUScanLinkageDesc::TABLE_TYPE_BAT);
      pmt.addElemStreamDesc(*ssusld);

      // AdaptationFieldDataDesc
      AdaptationFieldDataDesc* afdd = new AdaptationFieldDataDesc( AdaptationFieldDataDesc::PVR_ASSIST );
      pmt.addElemStreamDesc( *afdd );

      DUMP(pmt);
      pmt.buildSections(t);

      // dump built sections
      DUMP(t);

      return cmp_bin(t, "reference/pmt.ts");
   }
}
