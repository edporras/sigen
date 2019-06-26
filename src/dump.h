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
// debug.h: data dump routines.  This file is included in the
// library if ENABLE_DUMP is defined in the Makefile
// -----------------------------------

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "types.h"
#include "config.h"

#ifdef ENABLE_DUMP

namespace sigen {

   class LanguageCode;


   // output vars
   //
   enum STRID {
      IGNORE_S,           // NULL

      // identifier strings
      ADDITIONAL_INFO_S,  // Additional info
      ALIGNMENT_TYPE_S,   // Alignment type
      ANCILLARY_DATA_IDENT_S, // Ancillary data ident.
      ANCILLARY_PAGE_ID_S, // Ancillary pg. id
      ANNOUNCEMENT_SUPPORT_IND_S, // Announcement Support ind
      ANNOUNCEMENT_TYPE_S, // Announcement type
      ASVC_S,             // asvc
      ASVC_FLAG_S,        // asvc flag
      AUDIO_TYPE_S,       // Audio type
      BANDWIDTH_S,        // Bandwidth
      BOUND_VALID_F_S,    // Bound valid flag
      BOUQUET_ID_S,       // Bouquet id
      BOUQUET_DESC_LEN_S, // Bouquet desc len
      BOUQUET_NAME_S,     // Bouquet name
      BOUQUET_NAME_LEN_S, // Bouquet name len
      BSID_S,             // bsid
      BSID_FLAG_S,        // bsid_flag
      CA_PID_S,           // CA PID
      CA_SYSTEM_ID_S,     // CA system id
      CELL_ID_S,          // Cell id
      CELL_ID_EXT_S,      // Cell id ext.
      CELL_LATITUDE_S,    // Cell latitude
      CELL_LONGITUDE_S,   // Cell longitude
      CELL_EXT_LAT_S,     // Cell ext. of lat.
      CELL_EXT_LON_S,     // Cell ext. of lon.
      CHROMA_FORMAT_S,    // Chroma format
      CLOCK_ACCURACY_EXP_S, // Clock accuracy exp
      CLOCK_ACCURACY_INT_S, // Clock accuracy int
      CLOSED_GOP_F_S,     // Closed gop flag
      CODE_S,             // Code
      CODING_TYPE_S,      // Coding type
      COMPONENT_TAG_S,    // Component tag
      COMPONENT_TYPE_S,   // Component type
      COMPONENT_TYPE_FLAG_S, // Component type flag
      COMPOSITION_PAGE_ID_S, // Compositn. pg. id
      CONNECTION_TYPE_S,  // Connection type
      CONSTELLATION_S,    // Constellation
      CONSTR_PARAM_FLAG_S,// Cnstr. param. flag
      CONTENT_NL_1,       // Content nbl lvl 1
      CONTENT_NL_2,       // Content nbl lvl 2
      CORE_NUM_S,         // Core number
      CORE_NUM_LEN_S,     // Core number len
      COUNTRY_AVAIL_FLAG_S, // Cntry avail. flag
      COUNTRY_CODE_S,     // Country code
      COUNTRY_PREFIX_S,   // Country prefix
      COUNTRY_PREFIX_LEN_S, // Country prefix len
      COUNTRY_REGION_ID_S, // Country region id
      COPYRIGHT_IDENT_S,  // Copyright ident
      COPYRIGHT_INFO_S,   // Copyright info
      CR_HP_STREAM_S,     // Code rate HP stream
      CR_LP_STREAM_S,     // Code rate LP stream
      CURR_NEXT_IND_S,    // Current next ind.
      DATA_S,             // Data
      DATA_BCAST_ID_S,    // Data broadcast id
      DESC_LEN_S,         // Desc. len
      DESC_LOOP_LEN_S,    // Desc. loop len
      DESC_NUM_S,         // Desc. num
      DESCRPTN_S,         // Description
      DESCRPTN_LEN_S,     // Description len
      DURATION_S,         // Duration
      EIT_PF_F_S,         // EIT p/f
      EIT_SCHED_FLAG_S,   // EIT sched. flag
      ELEM_PID_S,         // Elementary PID
      ES_INFO_LEN_S,      // ES info len
      EVENT_ID_S,         // Event id
      EVENT_NAME_S,       // Event name
      EVENT_NAME_LEN_S,   // Event name len
      EXT_CLOCK_REF_IND_S,// Ext clock ref ind
      FEC_I_S,            // FEC inner
      FEC_O_S,            // FEC outer
      FOREIGN_AVAIL_S,    // Foreign avail.
      FORMAT_IDENT_S,     // Format identifier
      FRM_RATE_CODE_S,    // Frame rate code
      FRM_RATE_EXT_FLAG_S,// Frame rate ext flag
      FREE_CA_MODE_S,     // Free CA mode
      FREE_FRMT_FLAG_S,   // Free format flag
      FREQ_S,             // Frequency
      GUARD_INTERV_S,     // Guard interval
      HAND_OVER_TYPE_S,   // Hand-over type
      H_EMBEDDED_LAYER_S, // Hierarchy emb layer
      H_INFO_S,           // Hierarchy info
      H_LAYER_IDX_S,      // Hierarchy layer idx
      H_PRIORITY_S,       // Hierarchy priority
      H_TYPE_S,           // Hierarchy type
      HORIZ_OFFSET_S,     // Horizontal offset
      HORIZ_SIZE_S,       // Horizontal size
      ID_S,               // Id
      IDENT_INFO_S,       // Identification info
      IDENTICAL_GOP_F_S,  // Identical gop flag
      INITIAL_SERV_ID_S,  // Initial service id
      INTNL_AC_S,         // Intnl area code
      INTNL_AC_LEN_S,     // Intnl a/c len
      ITEM_S,             // Item
      ITEM_LEN_S,         // Item len
      LANGUAGE_CODE_S,    // Language code
      LAST_DESC_NUM_S,    // Last desc num
      LAYER_S,            // Layer
      LEAK_VALID_FLAG_S,  // Leak valid flag
      LEN_S,              // Length
      LENGTH_OF_ITEMS_S,  // Length of items
      LINKAGE_TYPE_S,     // Linkage type
      LOCAL_TIME_OFFSET_S, // Local time offset
      LTO_POLARITY_S,     // Lcl time offset pol
      LTW_OFFSET_LB_S,    // LTW offset low bnd
      LTW_OFFSET_UB_S,    // LTW offset upp bnd
      MAGAZINE_NUM_S,     // Magazine number
      MAINID_S,           // mainid
      MAINID_FLAG_S,      // mainid flag
      MAX_BITRATE_S,      // Maximum bitrate
      MAX_GOP_LEN_S,      // Max gop len
      MAX_OS_BUFFER_S,    // Max over. smth. buf
      MDV_VALID_FLAG_S,   // Mdv valid flag
      MIN_OS_RATE_S,      // Min over. smth. rate
      MIXINFO_EXISTS_FLAG_S, // Mixinfo exists flag
      MOD_S,              // Modulation
      MPEG_1_ONLY_FLAG_S, // MPEG 1 only flag
      MPLEX_DELAY_VAR_S,  // Mplex delay var
      MPLEX_STRATEGY_S,   // Mplex strategy
      MULT_FRM_RATE_FLAG_S,// Mult. frm. rt flag
      NAME_LEN_S,         // Name length
      NTNL_AC_S,          // Ntnl area code
      NTNL_AC_LEN_S,      // Ntnl a/c len
      NETWORK_DESC_LEN_S, // Network desc. len
      NETWORK_ID_S,       // Network id
      NETWORK_NAME_S,     // Network name
      NEW_SERV_ID_S,      // New service id
      NEXT_TIME_OFFSET_S, // Next time offset
      OPERATOR_CODE_S,    // Operator code
      OPERATOR_CODE_LEN_S,// Operator code len
      ORB_POS_S,          // Orbital position
      ORIGIN_TYPE_S,      // Origin type
      ORIG_NETWORK_ID_S,  // Orig. network id
      OTHER_FREQ_FLAG_S,  // Other freq flag
      OUI_S,              // OUI
      OUI_DATA_LEN_S,     // OUI data len
      PAGE_NUM_S,         // Page number
      PCR_PID_S,          // PCR Pid
      PEAK_RATE_S,        // Peak rate
      PEL_ASPECT_R_S,     // Pel aspect ratio
      PID_S,              // Pid
      POL_S,              // Polarisation
      PROF_LVL_IND_S,     // Prof/level ind.
      PROG_IDENT_LABEL_S, // Programme idnt. lbl
      PROGRAM_INFO_LEN_S, // Program info len
      PROGRAM_NUM_S,      // Program number
      PROV_NAME_S,        // Provider name
      PROV_NAME_LEN_S,    // Provider name len
      PVT_DATA_S,         // Private data
      PVT_DATA_IND_S,     // Private data ind
      RATING_S,           // Rating
      REF_EVENT_ID_S,     // Ref. event id
      REF_SID_S,          // Ref. service id
      REF_TYPE_S,         // Ref. type
      RESERVED_S,         // reserved
      RESERVED_FU_S,      // rsvrd. future use
      RUNNING_STATUS_S,   // Running status
      SB_LEAK_RATE_S,     // SB leak rate
      SB_SIZE_S,          // SB size
      SECT_SYNTAX_IND_S,  // Section syn. ind.
      SELECTOR_S,         // Selector
      SELECTOR_LEN_S,     // Selector len
      SERVICE_ID_S,       // Service id
      SERVICE_NAME_S,     // Service name
      SERVICE_NAME_LEN_S, // Service name len
      SRC_ID_S,           // Source id
      SRC_NAME_S,         // Source name
      SRC_NAME_LEN_S,     // Source name len
      START_TIME_S,       // Start time
      STILL_PICT_FLAG_S,  // Still pic flag
      STREAM_CONTENT_S,   // Stream content
      STREAM_TYPE_S,      // Stream type
      SUBCELL_INFO_LOOP_LEN_S, // Subcell loop len
      SUBCELL_LAT_S,      // Subcell lat.
      SUBCELL_LON_S,      // Subcell lon.
      SUBCELL_EXT_LAT_S,  // Subcell ext. of lat.
      SUBCELL_EXT_LON_S,  // Subcell ext. of lon.
      SUBSTREAM_1_S,      // Substream 1
      SUBSTREAM_1_FLAG_S, // Substream 1 flag
      SUBSTREAM_2_S,      // Substream 2
      SUBSTREAM_2_FLAG_S, // Substream 2 flag
      SUBSTREAM_3_S,      // Substream 3
      SUBSTREAM_3_FLAG_S, // Substream 3 flag
      SYM_RATE_S,         // Symbol rate
      TAG_S,              // Tag
      TID_S,              // Table id
      TABLE_LEN_S,        // Table length
      TABLE_TYPE_S,       // Table type
      TEXT_S,             // Text
      TEXT_LEN_S,         // Text len
      TRANS_MODE_S,       // Transmission mode
      TYPE_S,             // Type
      UPDATE_TYPE_S,      // Update type
      UPDATE_VER_FLAG_S,  // Update type
      UPDATE_VER_S,       // Update type
      USER_NL_1,          // User nibble 1
      USER_NL_2,          // User nibble 2
      UTC_S,              // UTC
      VAR_RATE_AUD_IND_S, // Variable rate audio ind.
      VER_NUM_S,          // Version number,
      VERT_OFFSET_S,      // Vertical offset
      VERT_SIZE_S,        // Vertical size
      WEST_EAST_S,        // West/east flag
      WIN_PRIORITY_S,     // Window priority
      XPORT_DESC_LEN_S,   // Xport desc len
      XPORT_STREAM_ID_S,  // Xport stream id
      XS_LOOP_LEN_S,      // Xprt str loop len
      XPOSER_FREQ_S,      // Xposer frequency

      // table strings
      BAT_DUMP_S,         // -- BAT Dump --
      CAT_DUMP_S,         // -- CAT Dump --
      EIT_PF_ACTUAL_S,    // -- EIT PF Dump (Actual) --
      EIT_PF_OTHER_S,     // -- EIT PF Dump (Other) --
      EIT_ES_ACTUAL_S,    // -- EIT ES (Actual) --
      EIT_ES_OTHER_S,     // -- EIT ES (Other) Dump --
      NIT_DUMP_ACTUAL_S,  // -- NIT Dump (Actual) --
      NIT_DUMP_OTHER_S,   // -- NIT Dump (Other) --
      PAT_DUMP_S,         // -- PAT Dump --
      PMT_DUMP_S,         // -- PMT Dump --
      RST_DUMP_S,         // -- RST Dump --
      SDT_DUMP_ACTUAL_S,  // -- SDT Dump (Actual) --
      SDT_DUMP_OTHER_S,   // -- SDT Dump (Other) --
      ST_DUMP_S,          // -- ST Dump --
      TDT_DUMP_S,         // -- TDT Dump --
      TOT_DUMP_S,         // -- TOT Dump --

      F_EVENT_LIST_S,     // -* Following Event List *-
      P_EVENT_LIST_S,     // -* Present Event List *-
      SERVICE_LIST_S,     // -* Service List *-
      STREAM_LIST_S,      // -* Stream List *-
      XPORT_STREAM_S,     // *-- Transport Stream --*

      // descriptor strings
      AC3_D_S,            // - AC-3 Descriptor -
      ANCILLARY_DATA_D_S, // - Ancillary Data Descriptor -
      ANNOUNCEMENT_SUPPORT_D_S, // - Announcement Support Descriptor -
      AUDIO_STREAM_D_S,   // - Audio Stream Descriptor -
      BOUQUET_NAME_D_S,   // - Bouquet name Descriptor -
      CA_D_S,             // - CA Descriptor -
      CA_IDENT_D_S,       // - CA Identifier Descriptor -
      CABLE_DEL_SYS_D_S,  // - Cable Delivery System Descriptor -
      CELL_FREQ_LINK_D_S, // - Cell Frequency Link Descriptor -
      CELL_LIST_D_S,      // - Cell List Descriptor -
      COMPONENT_D_S,      // - Component Descriptor -
      CONTENT_D_S,        // - Content Descriptor -
      COPYRIGHT_D_S,      // - Copyright Descriptor -
      COUNTRY_AVAIL_D_S,  // - Country Availability Descriptor -
      DATA_BCAST_D_S,     // - Data Broadcast Descriptor -
      DATA_BCAST_ID_D_S,  // - Data Broadcast Id Descriptor -
      DATA_STREAM_ALIGNMENT_D_S, // - Data Stream Alignment Descriptor -
      DSNG_D_S,           // - DSNG Descriptor -
      EXTENDED_AC3_D_S,   // - Extended AC-3 Descriptor -
      EXTENDED_EVENT_D_S, // - Extended Event Descriptor -
      FREQ_LIST_D_S,      // - Frequency List Descriptor -
      HIERARCHY_D_S,      // - Hierarchy Descriptor -
      IBP_D_S,            // - IBP Descriptor -
      ISO_639_LANG_D_S,   // - ISO 639 Language Descriptor -
      LINKAGE_D_S,        // - Linkage Descriptor -
      LOCAL_TIME_OFFSET_D_S, // - Local Time Offset Descriptor -
      MAXIMUM_BITRATE_D_S,// - Maximum Bitrate Descriptor -
      MULTILING_BOUQUET_NAME_D_S, // - Multilingual Bouquet Name Descriptor -
      MULTILING_COMPONENT_D_S, // - Multilingual Component Descriptor -
      MULTILING_NET_NAME_D_S, // - Multilingual Network Name Descriptor -
      MULTILING_SERV_NAME_D_S, // - Multilingual Service Name Descriptor -
      MULTIPLEX_BUF_UTIL_D_S, // - Multiplex Buf Utilization Descriptor -
      NETWORK_NAME_D_S,   // - Network Name Descriptor -
      NVOD_REF_D_S,       // - NVOD Reference Descriptor -
      PARENTAL_RATING_D_S,// - Parental Rating Descriptor -
      PARTIAL_TS_D_S,     // - Partial Transport Stream Descriptor -
      PDC_D_S,            // - PDC Descriptor -
      PVT_DATA_IND_D_S,   // - Private Data Indicator Descriptor -
      PVT_DATA_SPEC_D_S,  // - Private Data Specifier Descriptor -
      REGISTRATION_D_S,   // - Registration Descriptor -
      SAT_DEL_SYS_D_S,    // - Satellite Delivery System Descriptor -
      SERV_D_S,           // - Service Descriptor -
      SERV_LIST_D_S,      // - Service List Descriptor -
      SERV_MOVE_D_S,      // - Service Move Descriptor -
      SHORT_EVENT_D_S,    // - Short Event Descriptor -
      SHORT_SMTHNG_BUF_D_S, // - Short Smoothing Buffer Descriptor -
      SMOOTHING_BUF_D_S,  // - Smoothing Buffer Descriptor -
      STD_D_S,            // - STD Descriptor -
      STREAM_IDENT_D_S,   // - Stream Identifier Descriptor -
      STUFFING_D_S,       // - Stuffing Descriptor -
      SUBTITLING_D_S,     // - Subtitling Descriptor -
      SYSTEM_CLOCK_D_S,   // - System Clock Descriptor -
      TARGET_BACKGROUND_GRID_D_S, // - Target Background Grid Descriptor -
      TELEPHONE_D_S,      // - Telephone Descriptor -
      TELETEXT_D_S,       // - Teletext Descriptor -
      TER_DEL_SYS_D_S,    // - Terrestrial Delivery System Descriptor -
      TIME_SHIFTED_SERV_D_S, // - Time Shifted Service Descriptor -
      TIME_SHIFTED_EVENT_D_S, // - Time Shifted Event Descriptor -
      VIDEO_STREAM_D_S,   // - Video Stream Descriptor -
      VIDEO_WINDOW_D_S,   // - Video Window Descriptor -
      XPORT_STREAM_D_S,   // - Transport Stream Descriptor -

      // num_strings
      NUM_STRING_IDS
   };

   // output utility functions
   void incOutLevel();
   void decOutLevel();
   void headerStr(std::ostream& o, STRID s, bool desc_str = true);

   void identStr(std::ostream& o, STRID s);
   void identStr(std::ostream& o, STRID s, ui32 val, bool dechex = false);
   void identStr(std::ostream& o, STRID s, const std::string& data, bool cr = true);
   void identStr(std::ostream& o, STRID s, const LanguageCode& lc, bool cr = true);
   void identStr(std::ostream& o, STRID s, const std::vector<ui8>& data, bool cr = true);

   void dumpData(std::ostream& o, const ui8* data, ui16 len);
   void dumpBytes(std::ostream& o, const std::vector<ui8>& data);
} // sigen namespace

  // enable dumps to cerr
  #define DUMP(o) o.dump(std::cerr)
#else
  #define DUMP(o) // disable dumps
#endif
