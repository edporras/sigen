// Copyright 1999-2019 Ed Porras
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// ssu_desc.cc: class definition for the DVB defined SSU descriptors
// -----------------------------------

#include <iostream>
#include <utility>
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

      for (const OUIData &oui : oui_list)
      {
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
      for (const OUIData &oui : oui_list)
      {
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
      std::unique_ptr<SSUDataBroadcastIdDesc::OUIData> oui_entry(new SSUDataBroadcastIdDesc::OUIData(oui, upd_type, uvf, uv, sel_bytes));

      // can we add it?
      if ( !incLength( oui_entry->length() ) )
         return false;

      OUI_data_len += oui_entry->length();
      oui_list.push_back( std::move(oui_entry) );
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

      for (const auto &oui : oui_list)
      {
         s.set24Bits( oui->OUI );
         s.set08Bits( rbits(oui->reserved, 0xf0) | oui->update_type );
         s.set08Bits( rbits(oui->reserved, 0xc0) | (oui->update_versioning_flag << 5) | oui->update_version );
         s.set08Bits( oui->selector_bytes.size() );
         if ( oui->selector_bytes.size() ) {
            s.setBits( oui->selector_bytes );
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
      for (const auto &oui : oui_list)
      {
         identStr(o, OUI_S, oui->OUI);
         identStr(o, RESERVED_S, rbits(oui->reserved, 0x0f));
         identStr(o, UPDATE_TYPE_S, oui->update_type);
         identStr(o, RESERVED_S, rbits(oui->reserved, 0x03));
         identStr(o, UPDATE_VER_FLAG_S, oui->update_versioning_flag);
         identStr(o, UPDATE_VER_S, oui->update_version);
         identStr(o, SELECTOR_LEN_S, oui->selector_bytes.size(), true);
         identStr(o, SELECTOR_S, oui->selector_bytes);
      }
      decOutLevel();

      identStr(o, PVT_DATA_S, private_data);
   }
#endif

} // namespace
