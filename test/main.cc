//
// usage examples
//

#include <iostream>
#include <sigen/sigen.h> // this header includes all the required object headers

using namespace sigen;

namespace data
{
    const ui16 network_pid = 0x20;
    const ui16 xport_stream_id = 0x10;
    const int num_services = 10;
}

//
// sample for building a stream with an NIT and a TDT
//
int main(int argc, char* argv[])
{
   enum { NIT_F = 0x001, SDT_F = 0x002, CAT_F = 0x004,
	  PAT_F = 0x008, PMT_F = 0x010, BAT_F = 0x020,
	  EIT_F = 0x040, TDT_F = 0x100, TOT_F = 0x200 };
   ui16 flags = 0;

   if (argc == 1)
      flags = 0xffff;
   else
      for (int i = 1; i < argc; i++)
      {
         std::string arg = argv[i];

	 if (arg == "-nit")
	    flags |= NIT_F;
	 else if (arg == "-sdt")
	    flags |= SDT_F;
	 else if (arg == "-cat")
	    flags |= CAT_F;
	 else if (arg == "-pat")
	    flags |= PAT_F;
	 else if (arg == "-pmt")
	    flags |= PMT_F;
	 else if (arg == "-bat")
	    flags |= BAT_F;
	 else if (arg == "-eit")
	    flags |= EIT_F;
	 else if (arg == "-tot")
	    flags |= TOT_F;
	 else if (arg == "-tdt")
	    flags |= TDT_F;
	 else if (arg == "-h")
         {
             std::cerr << argv[0] << " linked against sigen library v" << sigen::version() << std::endl;
             return -1;
         }
      }

   // sections are built onto here
   TStream t;

   if (flags & PAT_F)
   {
      // build the pat object..
      // current_next & reserved_bits default to true
      PAT pat( data::xport_stream_id, 0x01 );

      // override the default maximum section length.. (optional)
      pat.setMaxSectionLen( 900 );

      // add the services (programs).. first, set the network pid (required for
      // some systems)
      pat.addNetworkPid( data::network_pid );

      for (int i = 0; i < data::num_services; i++)
      {
	 // each service has an id and pmt pid associated with it..
	 // get it from somehwere..
	 ui16 service_id = 100 + i;
	 ui16 pmt_pid = 200 + i;

	 pat.addProgram( service_id, pmt_pid );
      }

      // for debug output
      DUMP(pat);

      // this builds the binary section data onto the TStream object
      pat.buildSections(t);
   }

   if (flags & PMT_F)
   {
      // build the pat object..
      // current_next & reserved_bits default to true
      PMT pmt( 100, 101, 0x01 );

      SubtitlingDesc *subtd = new SubtitlingDesc;
      subtd->addSubtitling( "eng", 0x01, 0x1000, 0x1010 );
      subtd->addSubtitling( "fre", 0x01, 0x1000, 0x1010 );
      subtd->addSubtitling( "deu", 0x01, 0x1000, 0x1010 );
      subtd->addSubtitling( "spa", 0x01, 0x1000, 0x1010 );
      subtd->addSubtitling( "rus", 0x01, 0x1000, 0x1010 );

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
      ac3d->setValue( AC3Desc::ASVC, 0x4 );
      ac3d->setValue( AC3Desc::BSID, 0x2 );

      pmt.addProgramDesc( *ac3d );

      AncillaryDataDesc *add = new AncillaryDataDesc(0x51);
      pmt.addProgramDesc( *add );

      // add a stream
      pmt.addElemStream( sigen::PMT::ES_ISO_IEC_11172_VIDEO, 0x22 );

      // and descriptors to it
      LinkageDesc *ld = new LinkageDesc( 0x001, 0x002, 0x003, 0xf );
      pmt.addElemStreamDesc( *ld );

      // dumpage
      DUMP(pmt);
      pmt.buildSections(t);
   }

   if (flags & NIT_F)
   {
      // NIT test
      NIT nit(0x100, NIT::ACTUAL, 0x01);
      nit.setMaxSectionLen( 300 );

      // add a network descriptor
      NetworkNameDesc *nnd = new NetworkNameDesc("my network");
      nit.addNetworkDesc( *nnd );

      // another one
      std::string s(259, 'c');
      NetworkNameDesc *nd2 = new NetworkNameDesc(s);
      nit.addNetworkDesc( *nd2 );

      StuffingDesc *stuff1 = new StuffingDesc( 'z', 13 );
      nit.addNetworkDesc( *stuff1 );


      // multiling net name
      MultilingualNetworkNameDesc *mlnnd = new MultilingualNetworkNameDesc;
      mlnnd->addLanguage("fre", "La France");
      mlnnd->addLanguage("spa", "La Francia");

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
      nit.addXportStreamDesc( *sid );

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

      DUMP(nit);
      nit.buildSections(t);
   }

   if (flags & SDT_F)
   {
      // SDT
      SDT sdt(0x20, 0x30, SDT::ACTUAL, 0x05);

      sdt.addService(200, true, true, 1, false);

      ServiceDesc *sd = new ServiceDesc( 0xfe,
					 "My provider name is OSCAR and the point of this is to test if really long strings are truncated correctly so I must add even more data here",
					 "my service name is HOMER and the point of this is to test if really really long strings are really really truncated correctly");
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
      UTC t2;
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
   }

   if (flags & BAT_F)
   {
      BAT bat(0x1000, 0x10);

      DUMP(bat);
      bat.buildSections(t);
   }


   if (flags & EIT_F)
   {
      PF_EIT eit(100, 0x333, 0x444, PF_EIT::ACTUAL, 0);

      DUMP(eit);
      eit.buildSections(t);
   }

   if (flags & TOT_F)
   {
      TOT tot; // TOT with current time in utc field
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
   }

   if (flags & CAT_F)
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
   }

   if (flags & TDT_F)
   {
      // TDT
      TDT tdt;

      DUMP(tdt);

      tdt.buildSections(t);
   }

   // debug
   DUMP(t);

   // we now have a TStream that holds a list<Section *>.. each
   // Section holds an ui8[] with the data. we can write it to an
   // ostream (file, etc) by using the write method defined by
   // Section. Alternatively, you can have direct access to the binary
   // data by invoking getBinaryData().. regardless, iterate throguh
   // the list and do whatever you need like so:
   for (std::list<Section *>::const_iterator s_iter = t.section_list.begin();
	s_iter != t.section_list.end();
	s_iter++)
   {
      // const Section& section = *(*s_iter);
      // const ui8* sec_data = section.getBinaryData();
   }

   return 0;
}
