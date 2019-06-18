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
// ssu_desc.h: class definition for the DVB defined SSU descriptors
// -----------------------------------

#pragma once

#include <list>
#include <string>
#include <vector>
#include "linkage_desc.h"
#include "pmt_desc.h"

namespace sigen {

   // ---------------------------
   // SSU Linkage Descriptor (0x09)
   //
   class SSULinkageDesc : public LinkageDesc
   {
   public:
      // constructor for linkage types 0x09
      SSULinkageDesc(ui16 xsid, ui16 onid, ui16 sid) :
         LinkageDesc(xsid, onid, sid, LinkageDesc::SSUS),
         OUI_data_length(0)
      { incLength( 1 ); }
      SSULinkageDesc() = delete;

      // utility
      bool addOUI(ui32 OUI, const std::vector<ui8>& bytes = {});
      bool setPrivateData(const std::vector<ui8>& bytes);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      struct OUIData {
         enum { BASE_LEN = 4 };

         ui32 OUI:24;
         std::vector<ui8> selector_bytes;

         OUIData(ui32 oui) :
            OUI(oui) { }
         OUIData(ui32 oui, const std::vector<ui8>& bytes) :
            OUI(oui), selector_bytes(bytes) { }

         ui8 length() const { return expected_length(selector_bytes); }
         static ui8 expected_length(const std::vector<ui8>& sel_bytes) { return BASE_LEN + sel_bytes.size(); }
      };

      ui8 OUI_data_length;
      std::list<SSULinkageDesc::OUIData> oui_list;
      std::vector<ui8> private_data;
   };


   // ---------------------------
   // SSU Scan Linkage Descriptor (0x0A)
   //
   class SSUScanLinkageDesc : public LinkageDesc
   {
   public:
      enum TableType{
         TABLE_TYPE_UNDEF = 0x00,
         TABLE_TYPE_NIT   = 0x01,
         TABLE_TYPE_BAT   = 0x02,
      };

      // constructor for linkage types 0x0A
      SSUScanLinkageDesc(ui16 xsid, ui16 onid, ui16 sid, TableType t_type) :
         LinkageDesc(xsid, onid, sid, LinkageDesc::TS_SSU_BAT_OR_NIT),
         table_type(t_type)
      { }
      SSUScanLinkageDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui8 table_type;
   };

   // ---------------------------
   // SSU Data Broadcast Id Descriptor
   //
   class SSUDataBroadcastIdDesc : public DataBroadcastIdDesc
   {
   public:
      enum { SSU_SERVICE_DATA_BROADCAST_ID = 0x000A };

      enum {
         UPDATE_TYPE_PROPIETARY               = 0x0,
         UPDATE_TYPE_STANDARD_UPDATE_CAROUSEL = 0x1,
         UPDATE_TYPE_SSU_WITH_UNT             = 0x2,
         UPDATE_TYPE_SSU_USING_RETURN_CHANNEL = 0x3,
      };

      // constructor
      SSUDataBroadcastIdDesc() :
         DataBroadcastIdDesc(SSU_SERVICE_DATA_BROADCAST_ID),
         OUI_data_len(0)
      {
         incLength(sizeof(OUI_data_len));
      }

      // utility
      bool addOUI(ui32 oui, ui8 upd_type, ui8 uvf, ui8 uv, const std::vector<ui8>& sel_bytes);
      bool setPrivateData(const std::string& priv_data);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const;
#endif

   private:
      // OUI data struct
      struct OUIData {
         enum { BASE_LEN = 6 };

         // data
         ui32 OUI:24;
         ui8 update_type:4;
         bool update_versioning_flag;
         ui8 update_version:5;
         std::vector<ui8> selector_bytes;

         // constructor
         OUIData(ui32 oui, ui8 upd_type, bool uvf, ui8 uv, const std::vector<ui8>& sel_bytes) :
            OUI(oui), update_type(upd_type), update_versioning_flag(uvf), update_version(uv),
            selector_bytes(sel_bytes)
         { }

         ui8 length() const { return expected_length(selector_bytes); }
         static ui8 expected_length(const std::vector<ui8>& sel_bytes) { return BASE_LEN + sel_bytes.size(); }
      };

      ui8 OUI_data_len;
      std::string private_data;

      // descriptor OUI list
      std::list<OUIData> oui_list;
   };
} // sigen namespace
