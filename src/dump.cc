// Copyright 1999-2020 Ed Porras
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
// dump.cc: data dump routines.  This file is included in the
// library if ENABLE_DUMP is defined in the Makefile
// -----------------------------------

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <sstream>
#include "dump.h"
#include "language_code.h"
#include "utc.h"

namespace sigen
{
   // used for display
   static uint_fast8_t level = 0;
   static const uint_fast8_t LINE_WIDTH = 16;
   static const uint_fast8_t T_STR_W = 20;

   enum DumpFormat { AUTO, DEC, HEX, DECHEX, TABLE, FIELDS, DESC };
   struct Label {
      std::string str;
      DumpFormat format = AUTO;
   };

   //
   // output strings
   static const std::map<STRID, Label> out_str = {
      { IGNORE_S, { "" } },

      // identifier strings
      { ADAP_FLD_DATA_IDENT_S, { "Adapt. field data ident.", DECHEX } },
      { ADDITIONAL_INFO_S, { "Additional info" } },
      { ALIGNMENT_TYPE_S, { "Alignment type" } },
      { ANCILLARY_DATA_IDENT_S, { "Ancill. data idnt." } },
      { ANCILLARY_PAGE_ID_S, { "Ancillary pg. id", HEX } },
      { ANNOUNCEMENT_SUPPORT_IND_S, { "Announc. sup. ind." } },
      { ANNOUNCEMENT_TYPE_S, { "Announcement type" } },
      { ASVC_S, { "asvc" } },
      { ASVC_FLAG_S, { "asvc flag" } },
      { AUDIO_TYPE_S, { "Audio type" } },
      { BANDWIDTH_S, { "Bandwidth" } },
      { BOUND_VALID_F_S, { "Bound valid flag" } },
      { BOUQUET_ID_S, { "Bouquet id" } },
      { BOUQUET_DESC_LEN_S, { "Bouquet desc len", DECHEX } },
      { BOUQUET_NAME_S, { "Bouquet name" } },
      { BOUQUET_NAME_LEN_S, { "Bouquet name len", DECHEX } },
      { BSID_S, { "bsid" } },
      { BSID_FLAG_S, { "bsid_flag" } },
      { CA_PID_S, { "CA PID", DECHEX } },
      { CA_SYSTEM_ID_S, { "CA system id", DECHEX } },
      { CELL_ID_S, { "Cell id" } },
      { CELL_ID_EXT_S, { "Cell id ext." } },
      { CELL_LATITUDE_S, { "Cell latitude", DECHEX } },
      { CELL_LONGITUDE_S, { "Cell longitude", DECHEX } },
      { CELL_EXT_LAT_S, { "Cell ext. of lat.", DECHEX } },
      { CELL_EXT_LON_S, { "Cell ext. of lon.", DECHEX } },
      { CHROMA_FORMAT_S, { "Chroma format" } },
      { CLOCK_ACCURACY_EXP_S, { "Clock accuracy exp" } },
      { CLOCK_ACCURACY_INT_S, { "Clock accuracy int" } },
      { CLOSED_GOP_F_S, { "Closed gop flag" } },
      { CODE_S, { "ISO 639 Code" } },
      { CODING_TYPE_S, { "Coding type" } },
      { COMPONENT_TAG_S, { "Component tag" } },
      { COMPONENT_TYPE_S, { "Component type" } },
      { COMPONENT_TYPE_FLAG_S, { "Component type flag" } },
      { COMPOSITION_PAGE_ID_S, { "Compositn. pg. id", HEX } },
      { CONNECTION_TYPE_S, { "Connection type" } },
      { CONSTELLATION_S, { "Constellation" } },
      { CONSTR_PARAM_FLAG_S, { "Cnstr. param. flag" } },
      { CONTENT_NL_1, { "Content nbl lvl 1" } },
      { CONTENT_NL_2, { "Content nbl lvl 2" } },
      { CORE_NUM_S, { "Core number" } },
      { CORE_NUM_LEN_S, { "Core number len", DECHEX } },
      { COUNTRY_AVAIL_FLAG_S, { "Cntry avail. flag" } },
      { COUNTRY_CODE_S, { "Country code" } },
      { COUNTRY_PREFIX_S, { "Country prefix" } },
      { COUNTRY_PREFIX_LEN_S, { "Country prefix len", DECHEX } },
      { COUNTRY_REGION_ID_S, { "Country region id" } },
      { COPYRIGHT_IDENT_S, { "Copyright ident", DECHEX } },
      { COPYRIGHT_INFO_S, { "Copyright info" } },
      { CR_HP_STREAM_S, { "Code rate HP stream" } },
      { CR_LP_STREAM_S, { "Code rate LP stream" } },
      { CURR_NEXT_IND_S, { "Current next ind." } },
      { DATA_S, { "Data" } },
      { DATA_BCAST_ID_S, { "Data broadcast id", DECHEX } },
      { DESC_LEN_S, { "Desc. len", DECHEX } },
      { DESC_LOOP_LEN_S, { "Desc. loop len", DECHEX } },
      { DESC_NUM_S, { "Desc. num" } },
      { DESCRPTN_S, { "Description" } },
      { DESCRPTN_LEN_S, { "Description len", DECHEX } },
      { DURATION_S, { "Duration" } },
      { EIT_PF_F_S, { "EIT p/f" } },
      { EIT_SCHED_FLAG_S, { "EIT sched. flag" } },
      { ELEM_PID_S, { "Elementary PID", DECHEX } },
      { ES_INFO_LEN_S, { "ES info len", DECHEX } },
      { EVENT_ID_S, { "Event id" } },
      { EVENT_NAME_S, { "Event name" } },
      { EVENT_NAME_LEN_S, { "Event name len", DECHEX } },
      { EXT_CLOCK_REF_IND_S, { "Ext clock ref ind" } },
      { FEC_I_S, { "FEC inner" } },
      { FEC_O_S, { "FEC outer" } },
      { FOREIGN_AVAIL_S, { "Foreign avail." } },
      { FORMAT_IDENT_S, { "Format identifier" } },
      { FRM_RATE_CODE_S, { "Frame rate code" } },
      { FRM_RATE_EXT_FLAG_S, { "Frame rate ext flag" } },
      { FREE_CA_MODE_S, { "Free CA mode" } },
      { FREE_FRMT_FLAG_S, { "Free format flag" } },
      { FREQ_S, { "Frequency", DECHEX } },
      { GUARD_INTERV_S, { "Guard interval" } },
      { HAND_OVER_TYPE_S, { "Hand-over type" } },
      { H_EMBEDDED_LAYER_S, { "Hierarchy emb layer" } },
      { H_INFO_S, { "Hierarchy info" } },
      { H_LAYER_IDX_S, { "Hierarchy layer idx" } },
      { H_PRIORITY_S, { "Hierarchy priority" } },
      { H_TYPE_S, { "Hierarchy type" } },
      { HORIZ_OFFSET_S, { "Horizontal offset" } },
      { HORIZ_SIZE_S, { "Horizontal size" } },
      { ID_S, { "Id" } },
      { IDENT_INFO_S, { "Identification info" } },
      { IDENTICAL_GOP_F_S, { "Identical gop flag" } },
      { INITIAL_SERV_ID_S, { "Initial service id", DECHEX } },
      { INTNL_AC_S, { "Intnl area code" } },
      { INTNL_AC_LEN_S, { "Intnl a/c len", DECHEX } },
      { ITEM_S, { "Item" } },
      { ITEM_LEN_S, { "Item len", DECHEX } },
      { LANGUAGE_CODE_S, { "Language code" } },
      { LAST_DESC_NUM_S, { "Last desc num" } },
      { LAYER_S, { "Layer" } },
      { LEAK_VALID_FLAG_S, { "Leak valid flag" } },
      { LEN_S, { "Length", DECHEX } },
      { LENGTH_OF_ITEMS_S, { "Length of items", DECHEX } },
      { LINKAGE_TYPE_S, { "Linkage type" } },
      { LOCAL_TIME_OFFSET_S, { "Local time offset", DECHEX } },
      { LOGICAL_CHAN_NUM_S, { "Logical chan num", DECHEX } },
      { LTO_POLARITY_S, { "Lcl time offset pol" } },
      { LTW_OFFSET_LB_S, { "LTW offset low bnd" } },
      { LTW_OFFSET_UB_S, { "LTW offset upp bnd" } },
      { MAGAZINE_NUM_S, { "Magazine number" } },
      { MAINID_S, { "mainid" } },
      { MAINID_FLAG_S, { "mainid flag" } },
      { MAX_BITRATE_S, { "Maximum bitrate", DECHEX } },
      { MAX_GOP_LEN_S, { "Max gop len", DECHEX } },
      { MAX_OS_BUFFER_S, { "Max over. smth. buf" } },
      { MDV_VALID_FLAG_S, { "Mdv valid flag" } },
      { MIN_OS_RATE_S, { "Min over. smth. rate" } },
      { MIXINFO_EXISTS_FLAG_S, { "Mix-info exists flag" } },
      { MOD_S, { "Modulation" } },
      { MOD_SYS_S, { "Modulation system" } },
      { MPE_FEC_IND_S, { "MPE-FEC ind." } },
      { MPEG_1_ONLY_FLAG_S, { "MPEG1 only flag" } },
      { MPLEX_DELAY_VAR_S, { "Mplex delay var" } },
      { MPLEX_STRATEGY_S, { "Mplex strategy" } },
      { MULT_FRM_RATE_FLAG_S, { "Mult. frm. rt flag" } },
      { NAME_LEN_S, { "Name length", DECHEX } },
      { NTNL_AC_S, { "Ntnl area code" } },
      { NTNL_AC_LEN_S, { "Ntnl a/c len", DECHEX } },
      { NETWORK_DESC_LEN_S, { "Network desc. len", DECHEX } },
      { NETWORK_ID_S, { "Network id", DECHEX } },
      { NETWORK_NAME_S, { "Network name" } },
      { NEW_SERV_ID_S, { "New service id", DECHEX } },
      { NEXT_TIME_OFFSET_S, { "Next time offset", DECHEX } },
      { OPERATOR_CODE_S, { "Operator code" } },
      { OPERATOR_CODE_LEN_S, { "Operator code len" } },
      { ORB_POS_S, { "Orbital position" } },
      { ORIGIN_TYPE_S, { "Origin type" } },
      { ORIG_NETWORK_ID_S, { "Orig. network id", DECHEX } },
      { OTHER_FREQ_FLAG_S, { "Other freq flag" } },
      { OUI_S, { "OUI" } },
      { OUI_DATA_LEN_S, { "OUI data len", DECHEX } },
      { PAGE_NUM_S, { "Page number" } },
      { PCR_PID_S, { "PCR Pid", DECHEX } },
      { PEAK_RATE_S, { "Peak rate" } },
      { PEL_ASPECT_R_S, { "Pel aspect ratio" } },
      { PID_S, { "Pid", DECHEX } },
      { POL_S, { "Polarisation" } },
      { PRIORITY_S, { "Priority" } },
      { PROF_LVL_IND_S, { "Prof/level ind." } },
      { PROG_IDENT_LABEL_S, { "Programme idnt. lbl" } },
      { PROGRAM_INFO_LEN_S, { "Program info len", DECHEX } },
      { PROGRAM_NUM_S, { "Program number", DECHEX } },
      { PROV_NAME_S, { "Provider name" } },
      { PROV_NAME_LEN_S, { "Provider name len", DECHEX } },
      { PVT_DATA_S, { "Private data" } },
      { PVT_DATA_IND_S, { "Private data ind", DECHEX } },
      { RATING_S, { "Rating" } },
      { REF_EVENT_ID_S, { "Ref. event id" } },
      { REF_SID_S, { "Ref. service id" } },
      { REF_TYPE_S, { "Ref. type" } },
      { RESERVED_S, { "reserved", HEX } },
      { RESERVED_FU_S, { "rsvrd. future use", HEX } },
      { ROLLOFF_S, { "Rolloff" } },
      { RUNNING_STATUS_S, { "Running status" } },
      { SB_LEAK_RATE_S, { "SB leak rate", DECHEX } },
      { SB_SIZE_S, { "SB size" } },
      { SECT_SYNTAX_IND_S, { "Section syn. ind." } },
      { SELECTOR_S, { "Selector" } },
      { SELECTOR_LEN_S, { "Selector len", DECHEX } },
      { SERVICE_ID_S, { "Service id", DECHEX } },
      { SERVICE_NAME_S, { "Service name" } },
      { SERVICE_NAME_LEN_S, { "Service name len", DECHEX } },
      { SRC_ID_S, { "Source id" } },
      { SRC_NAME_S, { "Source name" } },
      { SRC_NAME_LEN_S, { "Source name len", DECHEX } },
      { START_TIME_S, { "Start time" } },
      { STILL_PICT_FLAG_S, { "Still pic flag" } },
      { STREAM_CONTENT_S, { "Stream content" } },
      { STREAM_TYPE_S, { "Stream type" } },
      { SUBCELL_INFO_LOOP_LEN_S, { "Subcell loop len", DECHEX } },
      { SUBCELL_LAT_S, { "Subcell lat." } },
      { SUBCELL_LON_S, { "Subcell lon." } },
      { SUBCELL_EXT_LAT_S, { "Subcell ext. of lat." } },
      { SUBCELL_EXT_LON_S, { "Subcell ext. of lon." } },
      { SUBSTREAM_1_S, { "Substream 1" } },
      { SUBSTREAM_1_FLAG_S, { "Substream 1 flag" } },
      { SUBSTREAM_2_S, { "Substream 2" } },
      { SUBSTREAM_2_FLAG_S, { "Substream 2 flag" } },
      { SUBSTREAM_3_S, { "Substream 3" } },
      { SUBSTREAM_3_FLAG_S, { "Substream 3 flag" } },
      { SYM_RATE_S, { "Symbol rate" } },
      { TAG_S, { "Tag", HEX } },
      { TID_S, { "Table id" } },
      { TABLE_LEN_S, { "Table length", DECHEX } },
      { TABLE_TYPE_S, { "Table type" } },
      { TEXT_S, { "Text" } },
      { TEXT_LEN_S, { "Text len", DECHEX } },
      { TIME_SLICE_IND_S, { "Time slicing ind." } },
      { TRANS_MODE_S, { "Transmission mode" } },
      { TYPE_S, { "Type" } },
      { UPDATE_TYPE_S, { "Update type" } },
      { UPDATE_VER_FLAG_S, { "Update vers. flg" } },
      { UPDATE_VER_S, { "Update version" } },
      { USER_NL_1, { "User nibble 1", HEX } },
      { USER_NL_2, { "User nibble 2", HEX } },
      { UTC_S, { "UTC" } },
      { VAR_RATE_AUD_IND_S, { "Var rate audio ind." } },
      { VER_NUM_S, { "Version number" } },
      { VERT_OFFSET_S, { "Vertical offset" } },
      { VERT_SIZE_S, { "Vertical size" } },
      { WEST_EAST_S, { "West/east flag" } },
      { VISIBLE_SERV_F_S, { "Visible serv flag" } },
      { WIN_PRIORITY_S, { "Window priority" } },
      { XPORT_DESC_LEN_S, { "Xport desc len", DECHEX } },
      { XPORT_STREAM_ID_S, { "Xport stream id", DECHEX } },
      { XS_LOOP_LEN_S, { "Xprt str loop len", DECHEX } },
      { XPOSER_FREQ_S, { "Xposer frequency", DECHEX } },

      // table strings
      { BAT_DUMP_S, { "- BAT Dump -", TABLE } },
      { CAT_DUMP_S, { "- CAT Dump -", TABLE } },
      { EIT_PF_ACTUAL_S, { "- EIT PF Dump (Actual) -", TABLE } },
      { EIT_PF_OTHER_S, { "- EIT PF Dump (Other) -", TABLE } },
      { EIT_ES_ACTUAL_S, { "- EIT ES (Actual) -", TABLE } },
      { EIT_ES_OTHER_S, { "- EIT ES (Other) -", TABLE } },
      { NIT_DUMP_ACTUAL_S, { "- NIT Dump (Actual) -", TABLE } },
      { NIT_DUMP_OTHER_S, { "- NIT Dump (Other) -", TABLE } },
      { PAT_DUMP_S, { "- PAT Dump -", TABLE } },
      { PMT_DUMP_S, { "- PMT Dump -", TABLE } },
      { RST_DUMP_S, { "- RST Dump -", TABLE } },
      { SDT_DUMP_ACTUAL_S, { "- SDT Dump (Actual) -", TABLE } },
      { SDT_DUMP_OTHER_S, { "- SDT Dump (Other) -", TABLE } },
      { ST_DUMP_S, { "- ST Dump -", TABLE } },
      { TDT_DUMP_S, { "- TDT Dump -", TABLE } },
      { TOT_DUMP_S, { "- TOT Dump -", TABLE } },

      { F_EVENT_LIST_S, { "-* Following Event List *-", FIELDS } },
      { P_EVENT_LIST_S, { "-* Present Event List *-", FIELDS } },
      { SERVICE_LIST_S, { "-* Service List *-", FIELDS } },
      { STREAM_LIST_S, { "-* Stream List *-", FIELDS } },
      { XPORT_STREAM_S, { "*-- Transport Stream --*" , FIELDS} },

      // descriptor name strings
      { AC3_D_S, { "AC-3", DESC } },
      { ADAPTATION_FIELD_DATA_S, { "Adaptation Field Data", DESC } },
      { ANCILLARY_DATA_D_S, { "Ancillary Data", DESC } },
      { ANNOUNCEMENT_SUPPORT_D_S, { "Announcement Support", DESC } },
      { AUDIO_STREAM_D_S, { "Audio Stream", DESC } },
      { BOUQUET_NAME_D_S, { "Bouquet name", DESC } },
      { CA_D_S, { "CA", DESC } },
      { CA_IDENT_D_S, { "CA Identifier", DESC } },
      { CABLE_DEL_SYS_D_S, { "Cable Delivery System", DESC } },
      { CELL_FREQ_LINK_D_S, { "Cell Frequency Link", DESC } },
      { CELL_LIST_D_S, { "Cell List", DESC } },
      { COMPONENT_D_S, { "Component", DESC } },
      { CONTENT_D_S, { "Content", DESC } },
      { COPYRIGHT_D_S, { "Copyright", DESC } },
      { COUNTRY_AVAIL_D_S, { "Country Availability", DESC } },
      { DATA_BCAST_D_S, { "Data Broadcast", DESC } },
      { DATA_BCAST_ID_D_S, { "Data Broadcast Id", DESC } },
      { DATA_STREAM_ALIGNMENT_D_S, { "Data Stream Alignment", DESC } },
      { DSNG_D_S, { "DSNG", DESC } },
      { EACEM_STREAM_IDENT_D_S, { "EACEM Stream Identifier Desc", DESC } },
      { EXTENDED_AC3_D_S, { "Extended AC-3", DESC } },
      { EXTENDED_EVENT_D_S, { "Extended Event", DESC } },
      { FREQ_LIST_D_S, { "Frequency List", DESC } },
      { HIERARCHY_D_S, { "Hierarchy", DESC } },
      { IBP_D_S, { "IBP", DESC } },
      { ISO_639_LANG_D_S, { "ISO 639 Language", DESC } },
      { LINKAGE_D_S, { "Linkage", DESC } },
      { LOCAL_TIME_OFFSET_D_S, { "Local Time Offset", DESC } },
      { LOGICAL_CHAN_D_S, { "Logical Channel Descriptor", DESC } },
      { MAXIMUM_BITRATE_D_S, { "Maximum Bitrate", DESC } },
      { MULTILING_BOUQUET_NAME_D_S, { "Multilingual Bouquet Name", DESC } },
      { MULTILING_COMPONENT_D_S, { "Multilingual Component", DESC } },
      { MULTILING_NET_NAME_D_S, { "Multilingual Network Name", DESC } },
      { MULTILING_SERV_NAME_D_S, { "Multilingual Service Name", DESC } },
      { MULTIPLEX_BUF_UTIL_D_S, { "Multiplex Buff Utilization", DESC } },
      { NETWORK_NAME_D_S, { "Network Name", DESC } },
      { NVOD_REF_D_S, { "NVOD Reference", DESC } },
      { PARENTAL_RATING_D_S, { "Parental Rating", DESC } },
      { PARTIAL_TS_D_S, { "Partial Transport Stream", DESC } },
      { PDC_D_S, { "PDC", DESC } },
      { PVT_DATA_IND_D_S, { "Private Data Indicator", DESC } },
      { PVT_DATA_SPEC_D_S, { "Private Data Specifier", DESC } },
      { REGISTRATION_D_S, { "Registration", DESC } },
      { SAT_DEL_SYS_D_S, { "Satellite Delivery System", DESC } },
      { SERV_D_S, { "Service", DESC } },
      { SERV_LIST_D_S, { "Service List", DESC } },
      { SERV_MOVE_D_S, { "Service Move", DESC } },
      { SHORT_EVENT_D_S, { "Short Event", DESC } },
      { SHORT_SMTHNG_BUF_D_S, { "Short Smoothing Buffer", DESC } },
      { SMOOTHING_BUF_D_S, { "Smoothing Buffer", DESC } },
      { STD_D_S, { "STD", DESC } },
      { STREAM_IDENT_D_S, { "Stream Identifier", DESC } },
      { STUFFING_D_S, { "Stuffing", DESC } },
      { SUBTITLING_D_S, { "Subtitling", DESC } },
      { SYSTEM_CLOCK_D_S, { "System Clock", DESC } },
      { TARGET_BACKGROUND_GRID_D_S, { "Target Background Grid", DESC } },
      { TELEPHONE_D_S, { "Telephone", DESC } },
      { TELETEXT_D_S, { "Teletext", DESC } },
      { TER_DEL_SYS_D_S, { "Terrestrial Delivery System", DESC } },
      { TIME_SHIFTED_SERV_D_S, { "Time Shifted Service", DESC } },
      { TIME_SHIFTED_EVENT_D_S, { "Time Shifted Event", DESC } },
      { VIDEO_STREAM_D_S, { "Video Stream", DESC } },
      { VIDEO_WINDOW_D_S, { "Video Window", DESC } },
      { XPORT_STREAM_D_S, { "Transport Stream", DESC } },

      { UTIL_DESC_D_S, { "Utility Pseudo", DESC } },
   };

   //
   // dumps a single data line
   //
   static void dumpDataLine(std::ostream& o, const ui8* data, ui8 length)
   {
      uint_fast8_t i;

      o << std::hex;

      // display the data in byte mode
      for (i = 0; i < length; i++)
         o << std::setw(2) << static_cast<ui16>(data[i]) << " ";

      // fill any empty spaces (if length < LINE_WIDTH)
      for (; i < LINE_WIDTH; i++)
         o <<  "   ";

      // display the data in char mode
      o << "  ";

      for (i = 0; i < length; i++) {
         char c = data[i];

         // map unprintables to '.'
         if ((c < ' ') || (c > '~'))
            c = '.';

         o << std::setw(1) << c;
      }
      o << std::endl;
   }

   //
   // outputs spaces before a line based on the current indent level
   static void indent(std::ostream& o)
   {
      for (uint_fast8_t i = 0; i < level; i++)
         o << " ";
      o.setf(std::ios::left | std::ios::showbase);
      o << std::setfill(' ');
   }

   //
   // dumps data in binary / char mode
   //
   void dumpData(std::ostream& o, const ui8* data, ui16 length)
   {
      // set stdout to not show base and pad right with 0's
      std::ios::fmtflags f = o.flags();

      o.unsetf(std::ios::showbase);
      o.setf(std::ios::right);
      o << std::setfill('0');

      uint_fast16_t num_lines = (length / LINE_WIDTH) + 1;
      for (uint_fast16_t i = 0, rem = length; i < num_lines; i++, rem -= LINE_WIDTH) {
         // calculate the length of the line
         int len = (rem < LINE_WIDTH) ? rem : LINE_WIDTH;

         o << "[" << std::dec
           << std::setw(4)
           << (i * LINE_WIDTH) << "] ";

         // display it
         dumpDataLine(o,& data[i * LINE_WIDTH], len);
      }
      o << std::endl;

      o.flags( f );
   }

   //
   // dumps a ui8 vector
   //
   void dumpBytes(std::ostream& o, const std::vector<ui8>& v)
   {
      std::stringstream sout;

      sout << std::setfill('0');
      for (ui16 byte : v) {
         sout << std::setw(2) << std::hex << byte;
      }
      sout << std::endl;

      o << sout.str();
   }

   //
   // increase / decrease the indentation level
   void incOutLevel() { level++; }
   void decOutLevel() { level--; }

   //
   // functions for displaying a header string
   void headerStr(std::ostream& o, STRID s)
   {
      const Label& l = out_str.at(s);
      indent(o);
      o << "- "
        << l.str;
      if (l.format == DESC)
         o << " Descriptor";
      o << " -" << std::endl;
   }

   //
   // functions for displaying identifier strings
   static inline void identStr(std::ostream& o, const Label& l)
   {
      indent(o);
      o << std::setw(T_STR_W) << l.str << ": ";
   }

   static inline void identStr(std::ostream& o, const Label& l, const std::string& data,
                               bool cr, bool quotes)
   {
      identStr(o, l);

      if (quotes)
         o << "\"";
      o << data;
      if (quotes)
         o << "\"";
      if (cr)
         o << std::endl;
   }

   static inline void identStr(std::ostream& o, const Label& l, ui32 data)
   {
      identStr(o, l);
      if (l.format == AUTO) {
         if (data < 10)
            o << std::dec << data;
         else if (data < 256)
            o << std::hex << data;
         else
            o << std::dec << data << " (" << std::hex << data << ")";
      } else if (l.format == DEC)
         o << std::dec << data;
      else if (l.format == DECHEX)
         o << std::dec << data << " (" << std::hex << data << ")";
      else
         o << std::hex << data;

      o << std::endl;
   }

   void identStr(std::ostream& o, STRID s, ui32 data)
   {
      identStr(o, out_str.at(s), data);
   }

   void identStr(std::ostream& o, STRID s, const std::string& data, bool cr)
   {
      if (data.length())
         identStr(o, out_str.at(s), data, cr, true);
   }

   void identStr(std::ostream& o, STRID s, const LanguageCode& lc)
   {
      identStr(o, out_str.at(s), lc.str(), true, true);
   }

   void identStr(std::ostream& o, STRID s, const UTC& time)
   {
      identStr(o, out_str.at(s));
      o << time << std::endl;
   }

   void identStr(std::ostream& o, STRID s, const BCDTime& dur)
   {
      identStr(o, out_str.at(s));
      o << dur << std::endl;
   }

   void identStr(std::ostream& o, STRID s, const std::vector<ui8>& data)
   {
      if (!data.empty()) {
         std::stringstream data_str;

         data_str.unsetf(std::ios::showbase);
         data_str.setf(std::ios::right);

         data_str << std::setfill('0');
         for (ui16 byte : data)
            data_str << std::setw(2) << std::hex << byte;

         identStr(o, out_str.at(s), data_str.str(), true, false);
      }
   }
}
