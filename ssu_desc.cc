//
// ssu_desc.cc: class definition for the DVB defined SSU descriptors
// -----------------------------------

#include <iostream>
#include <list>
#include <string>
#include "descriptor.h"
#include "ssu_desc.h"
#include "tstream.h"
#include "util.h"

namespace sigen
{
   // ---------------------------------------
   // SSU linkage descriptor
   //
   bool SSULinkageDesc::addOUI(const OUIData& oui_data)
   {
      if (!incLength( oui_data.length() ))
         return false;

      oui_list.push_back(oui_data);
      OUI_data_length += oui_data.length();
      return true;
   }

   //
   // set the private bytes
   //
   bool SSULinkageDesc::setPrivateData(const std::vector<ui8>& bytes)
   {
      if (!incLength(bytes.size()))
         return false;

      private_data = bytes;
      return true;
   }


   //
   // write to the section
   //
   void SSULinkageDesc::buildSections(Section &s) const
   {
      LinkageDesc::buildSections(s);

      s.set08Bits( OUI_data_length );

      for ( std::list<SSULinkageDesc::OUIData>::const_iterator iter = oui_list.begin();
            iter != oui_list.end();
            iter++ )
      {
         const OUIData &oui = *iter;

         s.set24Bits( oui.OUI );
         s.set08Bits( oui.selector_bytes.size() );
         s.setBits( oui.selector_bytes );
      }

      s.setBits( private_data );
   }


#ifdef ENABLE_DUMP
   //
   // stdout
   void SSULinkageDesc::dump(std::ostream &o) const
   {
      LinkageDesc::dump(o);

      identStr( o, OUI_DATA_LEN_S, OUI_data_length );

      incOutLevel();
      for ( std::list<SSULinkageDesc::OUIData>::const_iterator iter = oui_list.begin();
            iter != oui_list.end();
            iter++ )
      {
         const OUIData &oui = *iter;

         identStr(o, OUI_S, oui.OUI);
         identStr(o, SELECTOR_LEN_S, oui.selector_bytes.size(), true);
         identStr(o, SELECTOR_S, oui.selector_bytes);
      }

      decOutLevel();
      identStr(o, PVT_DATA_S, private_data);
   }
#endif


   // ---------------------------------------
   // SSU Scan linkage descriptor
   //

   //
   // write to the section
   //
   void SSUScanLinkageDesc::buildSections(Section &s) const
   {
      LinkageDesc::buildSections(s);
      s.set08Bits( table_type );
   }


#ifdef ENABLE_DUMP
   //
   // stdout
   void SSUScanLinkageDesc::dump(std::ostream &o) const
   {
      LinkageDesc::dump(o);
      identStr( o, TABLE_TYPE_S, table_type );
   }
#endif



   // ---------------------------------------
   // SSU Data Broadcast Id Descriptor
   //
   bool SSUDataBroadcastIdDesc::addOUI(ui32 oui, ui8 upd_type, ui8 uvf, ui8 uv, const std::vector<ui8>& sel_bytes)
   {
      SSUDataBroadcastIdDesc::OUIData oui_entry(oui, upd_type, uvf, uv, sel_bytes);

      // can we add it?
      if ( !incLength( oui_entry.length() ) )
         return false;

      OUI_data_len += oui_entry.length();
      oui_list.push_back( oui_entry );
      return true;
   }


   bool SSUDataBroadcastIdDesc::setPrivateData(const std::string& priv_data)
   {
      if ( !incLength( priv_data.length() ) )
         return false;

      private_data = priv_data;
      return true;
   }


   void SSUDataBroadcastIdDesc::buildSections(Section& s) const
   {
      DataBroadcastIdDesc::buildSections(s);

      s.set08Bits( OUI_data_len );

      for ( std::list<SSUDataBroadcastIdDesc::OUIData>::const_iterator iter = oui_list.begin();
            iter != oui_list.end();
            iter++ )
      {
         const OUIData &oui = *iter;

         s.set24Bits( oui.OUI );
         s.set08Bits( rbits(oui.reserved, 0xf0) | oui.update_type );
         s.set08Bits( rbits(oui.reserved, 0xc0) | (oui.update_versioning_flag << 5) | oui.update_version );
         s.set08Bits( oui.selector_bytes.size() );
         if ( oui.selector_bytes.size() ) {
            s.setBits( oui.selector_bytes );
         }
      }

      s.setBits( private_data );
   }

#ifdef ENABLE_DUMP
   void SSUDataBroadcastIdDesc::dump(std::ostream& o) const
   {
      DataBroadcastIdDesc::dump(o);

      identStr(o, OUI_DATA_LEN_S, OUI_data_len);

      incOutLevel();
      for ( std::list<SSUDataBroadcastIdDesc::OUIData>::const_iterator iter = oui_list.begin();
            iter != oui_list.end();
            iter++ )
      {
         const OUIData &oui = *iter;

         identStr(o, OUI_S, oui.OUI);
         identStr(o, RESERVED_S, rbits(oui.reserved, 0x0f));
         identStr(o, UPDATE_TYPE_S, oui.update_type);
         identStr(o, RESERVED_S, rbits(oui.reserved, 0x03));
         identStr(o, UPDATE_VER_FLAG_S, oui.update_versioning_flag);
         identStr(o, UPDATE_VER_S, oui.update_version);
         identStr(o, SELECTOR_LEN_S, oui.selector_bytes.size(), true);
         identStr(o, SELECTOR_S, oui.selector_bytes);
      }
      decOutLevel();

      identStr(o, PVT_DATA_S, private_data);
   }
#endif

} // namespace