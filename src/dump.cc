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
// debug.c: data dump routines.  This file is included in the
// library if ENABLE_DUMP is defined in the Makefile
// -----------------------------------

#ifdef ENABLE_DUMP

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include "dump.h"
#include "util.h"

namespace sigen
{
    namespace dump_priv {
        // used for display
        ui8 level = 0;
        const int LINE_WIDTH = 16;
        const int T_STR_W = 20;

        //
        // output strings
        const std::string outstr[ NUM_STRING_IDS ] = {
            "",                  // IGNORE_S

            // identifier strings
            "AC-3 type",                 // AC3TYPE_S
            "AC-3 type flag",            // AC3TYPE_FLAG_S
            "Additional info",           // ADDITIONAL_INFO_S
            "Alignment type",            // ALIGNMENT_TYPE_S
            "Ancill. data idnt.",        // ANCILLARY_DATA_IDENT_S
            "Ancillary pg. id",          // ANCILLARY_PAGE_ID_S
            "Announc. sup. ind.",        // ANNOUNCEMENT_SUPPORT_IND_S
            "Announcement type",         // ANNOUNCEMENT_TYPE_S
            "asvc",                      // ASVC_S
            "asvc flag",                 // ASVC_FLAG_S
            "Audio type",                // AUDIO_TYPE_S
            "Bandwidth",                 // BANDWIDTH_S
            "Bound valid flag",          // BOUND_VALID_F_S
            "Bouquet id",                // BOUQUET_ID_S
            "Bouquet desc len",          // BOUQUET_DESC_LEN_S
            "Bouquet name",              // BOUQUET_NAME_S
            "Bouquet name len",          // BOUQUET_NAME_LEN_S
            "bsid",                      // BSID_S
            "bsid_flag",                 // BSID_FLAG_S
            "CA PID",                    // CA_PID_S
            "CA system id",              // CA_SYSTEM_ID_S
            "Cell id",                   // CELL_ID_S
            "Cell id ext.",              // CELL_ID_EXT_S
            "Cell latitude",             // CELL_LATITUDE_S
            "Cell longitude",            // CELL_LONGITUDE_S
            "Cell ext. of lat.",         // CELL_EXT_LAT_S
            "Cell ext. of lon.",         // CELL_EXT_LON_S
            "Chroma format",             // CHROMA_FORMAT_S
            "Clock accuracy exp",        // CLOCK_ACCURACY_EXP_S
            "Clock accuracy int",        // CLOCK_ACCURACY_INT_S
            "Closed gop flag",           // CLOSED_GOP_F_S
            "ISO 639 Code",              // CODE_S
            "Component tag",             // COMPONENT_TAG_S
            "Component type",            // COMPONENT_TYPE_S
            "Compositn. pg. id",         // COMPOSITION_PAGE_ID_S
            "Connection type",           // CONNECTION_TYPE_S
            "Constellation",             // CONSTELLATION_S
            "Cnstr. param. flag",        // CONSTR_PARAM_FLAG_S
            "Content nbl lvl 1",         // CONTENT_NL_1
            "Content nbl lvl 2",         // CONTENT_NL_2
            "Core number",               // CORE_NUM_S
            "Core number len",           // CORE_NUM_LEN_S
            "Cntry avail. flag",         // COUNTRY_AVAIL_FLAG_S
            "Country code",              // COUNTRY_CODE_S
            "Country prefix",            // COUNTRY_PREFIX_S
            "Country prefix len",        // COUNTRY_PREFIX_LEN_S
            "Country region id",         // COUNTRY_REGION_ID_S
            "Copyright ident",           // COPYRIGHT_IDENT_S
            "Copyright info",            // COPYRIGHT_INFO_S
            "Code rate HP stream",       // CR_HP_STREAM_S
            "Code rate LP stream",       // CR_LP_STREAM_S
            "Coding type",               // CODING_TYPE_S
            "Current next ind.",         // CURR_NEXT_IND_S
            "Data",                      // DATA_S
            "Data broadcast id",         // DATA_BCAST_ID_S
            "Desc. len",                 // DESC_LEN_S
            "Desc. loop len",            // DESC_LOOP_LEN_S
            "Desc. num",                 // DESC_NUM_S
            "Description",               // DESCRPTN_S
            "Description len",           // DESCRPTN_LEN_S
            "Duration",                  // DURATION_S
            "EIT p/f",                   // EIT_PF_F_S
            "EIT sched. flag",           // EIT_SCHED_F_S
            "Elementary PID",            // ELEM_PID_S
            "ES info len",               // ES_INFO_LEN_S
            "Event id",                  // EVENT_ID_S
            "Event name",                // EVENT_NAME_S
            "Event name len",            // EVENT_NAME_LEN_S
            "Ext clock ref ind",         // EXT_CLOCK_REF_IND_S
            "FEC inner",                 // FEC_I_S
            "FEC outer",                 // FEC_O_S
            "Foreign avail.",            // FOREIGN_AVAIL_S
            "Format identifier",         // FORMAT_IDENT_S
            "Frame rate code",           // FRM_RATE_CODE_S
            "Frame rate ext flag",       // FRM_RATE_EXT_FLAG_S
            "Free CA mode",              // FREE_CA_MODE_S
            "Free format flag",          // FREE_FRMT_FLAG_S
            "Frequency",                 // FREQ_S
            "Guard interval",            // GUARD_INTERV_S
            "Hand-over type",            // HAND_OVER_TYPE_S
            "Hierarchy emb layer",       // H_EMBEDDED_LAYER_S
            "Hierarchy info",            // H_INFO_S
            "Hierarchy layer idx",       // H_LAYER_IDX_S
            "Hierarchy priority",        // H_PRIORITY_S
            "Hierarchy type",            // H_TYPE_S
            "Horizontal offset",         // HORIZ_OFFSET_S
            "Horizontal size",           // HORIZ_SIZE_S
            "Id",                        // ID_S
            "Identification info",       // IDENT_INFO_S
            "Identical gop flag",        // IDENTICAL_GOP_F_S
            "Initial service id",        // INITIAL_SERV_ID_S
            "Intnl area code",           // INTNL_AC_S
            "Intnl a/c len",             // INTNL_AC_LEN_S
            "Item",                      // ITEM_S
            "Item len",                  // ITEM_LEN_S
            "Language code",             // LANGUAGE_CODE_S
            "Last desc num",             // LAST_DESC_NUM_S
            "Layer",                     // LAYER_S
            "Leak valid flag",           // LEAK_VALID_FLAG_S
            "Length",                    // LEN_S
            "Length of items",           // LENGTH_OF_ITEMS_S
            "Linkage type",              // LINKAGE_TYPE_S
            "Local time offset",         // LOCAL_TIME_OFFSET_S
            "Lcl time offset pol",       // LTO_POLARITY_S
            "LTW offset low bnd",        // LTW_OFFSET_LB_S
            "LTW offset upp bnd",        // LTW_OFFSET_UB_S
            "Magazine number",           // MAGAZINE_NUM_S
            "mainid",                    // MAINID_S
            "mainid flag",               // MAINID_FLAG_S
            "Maximum bitrate",           // MAX_BITRATE_S
            "Max gop len",               // MAX_GOP_LEN_S
            "Max over. smth. buf",       // MAX_OS_BUFFER_S
            "Mdv valid flag",            // MDV_VALID_FLAG_S
            "Min over. smth. rate",      // MIN_OS_RATE_S
            "Modulation",                // MOD_S
            "MPEG2 flag",                // MPEG_2_FLAG_S
            "Mplex delay var",           // MPLEX_DELAY_VAR_S
            "Mplex strategy",            // MPLEX_STRATEGY_S
            "Mult. frm. rt flag",        // MULT_FRM_RATE_FLAG_S
            "Name length",               // NAME_LEN_S
            "Ntnl area code",            // NTNL_AC_S
            "Ntnl a/c len",              // NTNL_AC_LEN_S
            "Network desc. len",         // NETWORK_DESC_LEN_S
            "Network id",                // NETWORK_ID_S
            "Network name",              // NETWORK_NAME_S
            "New service id",            // NEW_SERV_ID_S
            "Next time offset",          // NEXT_TIME_OFFSET_S
            "Operator code",             // OPERATOR_CODE_S
            "Operator code len",         // OPERATOR_CODE_LEN_S
            "Orbital position",          // ORB_POS_S
            "Origin type",               // ORIGIN_TYPE_S
            "Orig. network id",          // ORIG_NETWORK_ID_S
            "Other freq flag",           // OTHER_FREQ_FLAG_S
            "OUI",                       // OUI_S
            "OUI data len",              // OUI_DATA_LEN_S
            "Page number",               // PAGE_NUM_S
            "PCR Pid",                   // PCR_PID_S
            "Peak rate",                 // PEAK_RATE_S
            "Pel aspect ratio",          // PEL_ASPECT_R_S
            "Pid",                       // PID_S
            "Polarisation",              // POL_S
            "Prof/level ind.",           // PROF_LVL_IND_S
            "Programme idnt. lbl",       // PROG_IDENT_LABEL_S
            "Program info len",          // PROGRAM_INFO_LEN_S
            "Program number",            // PROGRAM_NUM_S
            "Provider name",             // PROV_NAME_S
            "Provider name len",         // PROV_NAME_LEN_S
            "Private data",              // PVT_DATA_S
            "Private data ind",          // PVT_DATA_IND_S
            "Rating",                    // RATING_S
            "Ref. event id",             // REF_EVENT_ID_S
            "Ref. service id",           // REF_SID_S
            "Ref. type",                 // REF_TYPE_S
            "reserved",                  // RESERVED_S
            "rsvrd. future use",         // RESERVED_FU_S
            "Running status",            // RUNNING_STATUS_S
            "SB leak rate",              // SB_LEAK_RATE_S
            "SB size",                   // SB_SIZE_S
            "Section syn. ind.",         // SECT_SYNTAX_IND_S
            "Selector",                  // SELECTOR_S
            "Selector len",              // SELECTOR_LEN_S
            "Service id",                // SERVICE_ID_S
            "Service name",              // SERVICE_NAME_S
            "Service name len",          // SERVICE_NAME_LEN_S
            "Source id",                 // SRC_ID_S
            "Source name",               // SRC_NAME_S
            "Source name len",           // SRC_NAME_LEN_S
            "Start time",                // START_TIME_S
            "Still pic flag",            // STILL_PICT_FLAG_S
            "Stream content",            // STREAM_CONTENT_S
            "Stream type",               // STREAM_TYPE_S
            "Subcell loop len",          // SUBCELL_INFO_LOOP_LEN_S
            "Subcell lat.",              // SUBCELL_LAT_S
            "Subcell lon.",              // SUBCELL_LON_S
            "Subcell ext. of lat.",      // SUBCELL_EXT_LAT_S
            "Subcell ext. of lon.",      // SUBCELL_EXT_LON_S
            "Symbol rate",               // SYM_RATE_S
            "Tag",                       // TAG_S
            "Table id",                  // TID_S
            "Table length",              // TABLE_LEN_S
            "Table type",                // TABLE_TYPE_S
            "Text",                      // TEXT_S
            "Text len",                  // TEXT_LEN_S
            "Transmission mode",         // TRANS_MODE_S
            "Type",                      // TYPE_S
            "Update type",               // UPDATE_TYPE_S
            "Update vers. flg",          // UPDATE_VER_FLAG_S
            "Update version",            // UPDATE_VER_S
            "User nibble 1",             // USER_NL_1
            "User nibble 2",             // USER_NL_2
            "UTC",                       // UTC_S
            "Version number",            // VER_NUM_S
            "Vertical offset",           // VERT_OFFSET_S
            "Vertical size",             // VERT_SIZE_S
            "West/east flag",            // WEST_EAST_S
            "Window priority",           // WIN_PRIORITY_S
            "Xport desc len",            // XPORT_DESC_LEN_S
            "Xport stream id",           // XPORT_STREAM_ID_S
            "Xprt str loop len",         // XS_LOOP_LEN_S
            "Xposer frequency",          // XPOSER_FREQ_S

            // table strings
            "- BAT Dump -",              // BAT_DUMP_S
            "- CAT Dump -",              // CAT_DUMP_S
            "- EIT PF Dump (Actual) -",  // EIT_PF_ACTUAL_S
            "- EIT PF Dump (Other) -",   // EIT_PF_OTHER_S
            "- EIT ES (Actual) -",       // EIT_ES_ACTUAL_S
            "- EIT ES (Other) -",        // EIT_ES_OTHER_S
            "- NIT Dump (Actual) -",     // NIT_DUMP_S
            "- NIT Dump (Other) -",      // NIT_DUMP_S
            "- PAT Dump -",              // PAT_DUMP_S
            "- PMT Dump -",              // PMT_DUMP_S
            "- RST Dump -",              // RST_DUMP_S
            "- SDT Dump (Actual) -",     // SDT_DUMP_S
            "- SDT Dump (Other) -",      // SDT_DUMP_S
            "- ST Dump -",               // ST_DUMP_S
            "- TDT Dump -",              // TDT_DUMP_S
            "- TOT Dump -",              // TOT_DUMP_S

            "-* Following Event List *-", // F_EVENT_LIST_S
            "-* Present Event List *-",  // P_EVENT_LIST_S
            "-* Service List *-",        // SERVICE_LIST_S
            "-* Stream List *-",         // STREAM_LIST_S
            "*-- Transport Stream --*",  // XPORT_STREAM_S

            // descriptor name strings
            "AC-3",                      // AC3_D_S
            "Ancillary Data",            // ANCILLARY_DATA_D_S
            "Announcement Support",      // ANNOUNCEMENT_SUPPORT_D_S
            "Audio Stream",              // AUDIO_STREAM_D_S
            "Bouquet name",              // BOUQUET_NAME_D_S
            "CA",                        // CA_D_S
            "CA Identifier",             // CA_IDENT_D_S
            "Cable Delivery System",     // CABLE_DEL_SYS_D_S
            "Cell Frequency Link",       // CELL_FREQ_LINK_D_S
            "Cell List",                 // CELL_LIST_D_S
            "Component",                 // COMPONENT_D_S
            "Content",                   // CONTENT_D_S
            "Copyright",                 // COPYRIGHT_D_S
            "Country Availability",      // COUNTRY_AVAIL_D_S
            "Data Broadcast",            // DATA_BCAST_D_S
            "Data Broadcast Id",         // DATA_BCAST_ID_D_S
            "Data Stream Alignment",     // DATA_STREAM_ALIGNMENT_D_S
            "DSNG",                      // DSNG_D_S
            "Extended Event",            // EXTENDED_EVENT_D_S
            "Frequency List",            // FREQ_LIST_D_S
            "Hierarchy",                 // HIERARCHY_D_S
            "IBP",                       // IBP_D_S
            "ISO 639 Language",          // ISO_639_LANG_D_S
            "Linkage",                   // LINKAGE_D_S
            "Local Time Offset",         // LOCAL_TIME_OFFSET_D
            "Maximum Bitrate",           // MAXIMUM_BITRATE_D_S
            "Multilingual Bouquet Name", // MULTILING_BOUQUET_NAME_D_S
            "Multilingual Component",    // MULTILING_COMPONENT_D_S
            "Multilingual Network Name", // MULTILING_NET_NAME_D_S
            "Multilingual Service Name", // MULTILING_SERV_NAME_D_S
            "Multiplex Buff Utilization", // MULTIPLEX_BUF_UTIL_D_S
            "Network Name",              // NETWORK_NAME_D_S
            "NVOD Reference",            // NVOD_REF_D_S
            "Parental Rating",           // PARENTAL_RATING_D_S
            "Partial Transport Stream",  // PARTIAL_XS_D_S
            "PDC",                       // PDC_D_S
            "Private Data Indicator",    // PVT_DATA_IND_D_S
            "Private Data Specifier",    // PVT_DATA_SPEC_D_S
            "Registration",              // REGISTRATION_D_S
            "Satellite Delivery System", // SAT_DEL_SYS_D_S
            "Service",                   // SERV_D_S
            "Service List",              // SERV_LIST_D_S
            "Service Move",              // SERV_MOVE_D_S
            "Short Event",               // SHORT_EVENT_D_S
            "Short Smoothing Buffer",    // SHORT_SMTHNG_BUF_D_S
            "Smoothing Buffer",          // SMOOTHING_BUF_D_S
            "STD",                       // STD_D_S
            "Stream Identifier",         // STREAM_IDENT_D_S
            "Stuffing",                  // STUFFING_D_S
            "Subtitling",                // SUBTITLING_D_S
            "System Clock",              // SYSTEM_CLOCK_D_S
            "Target Background Grid",    // TARGET_BACKGRONUD_GRID_D_S
            "Telephone",                 // TELEPHONE_D_S
            "Teletext",                  // TELETEXT_D_S
            "Terrestrial Delivery System", // TER_DEL_SYS_D_S
            "Time Shifted Service",      // TIME_SHIFTED_D_S
            "Time Shifted Event",        // TIME_SHIFTED_EVENT_D_S
            "Video Stream",              // VIDEO_STREAM_D_S
            "Video Window",              // VIDEO_WINDOW_D_S
            "Transport Stream",          // XPORT_STREAM_D_S
        };

        void decHexDisplay(std::ostream &o, ui16 v);
        void dumpDataLine(std::ostream &o, const ui8 *data, ui8 length);
        void indent(std::ostream &o);
    }

    using namespace dump_priv;

    //
    // displays a value in dec and hex
    //
    void dump_priv::decHexDisplay(std::ostream &o, ui16 v)
    {
        o << std::dec << v << " (" << std::hex << v << ")";
    }

    //
    // dumps a single data line
    //
    void dump_priv::dumpDataLine(std::ostream &o, const ui8 *data, ui8 length)
    {
        ui8 i;

        o << std::hex;

        // display the data in byte mode
        for (i = 0; i < length; i++)
            o << std::setw(2) << (ui16) data[i] << " ";

        // fill any empty spaces (if length < LINE_WIDTH)
        for (; i < LINE_WIDTH; i++)
            o <<  "   ";

        // display the data in char mode
        o << "  ";

        for (i = 0; i < length; i++)
        {
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
    void dump_priv::indent(std::ostream &o)
    {
        for (ui8 i = 0; i < level; i++)
            o << " ";
        o.setf(std::ios::left | std::ios::showbase);
        o << std::setfill(' ');
    }



    //
    // dumps data in binary / char mode
    //
    void dumpData(std::ostream &o, const ui8 *data, ui16 length)
    {
        int num_lines = (length / LINE_WIDTH) + 1;

        // set stdout to not show base and pad right with 0's
        std::ios::fmtflags f = o.flags();

        o.unsetf(std::ios::showbase);
        o.setf(std::ios::right);
        o << std::setfill('0');

        for (int i = 0, rem = length; i < num_lines; i++, rem -= LINE_WIDTH)
        {
            // calculate the length of the line
            int len = (rem < LINE_WIDTH) ? rem : LINE_WIDTH;

            o << "[" << std::dec
              << std::setw(4)
              << (i * LINE_WIDTH) << "] ";

            // display it
            dumpDataLine(o, &data[i * LINE_WIDTH], len);
        }
        o << std::endl;

        o.flags( f );
    }


    //
    // dumps a ui8 vector
    //
    void dumpBytes(std::ostream &o, const std::vector<ui8>& v)
    {
        std::stringstream sout;

        sout << std::setfill('0');
        for (ui32 i = 0; i < v.size(); i++)
        {
            sout << std::setw(2) << std::hex << (ui16) v[i];
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
    void headerStr(std::ostream &o, const std::string &s, bool desc_str)
    {
        indent(o);
        o << "- "
          << s;
        if (desc_str)
            o << " Descriptor";
        o << " -" << std::endl;
    }

    void headerStr(std::ostream &o, STRID s, bool desc_str)
    {
        headerStr(o, outstr[s], desc_str);
    }

    //
    // functions for displaying identifier strings
    void identStr(std::ostream &o, const std::string &s)
    {
        indent(o);
        o << std::setw(T_STR_W) << s.c_str() << ": ";
    }

    void identStr(std::ostream &o, const std::string &s, const std::string &data, bool cr, bool quotes)
    {
        identStr(o, s);

        if (quotes)
            o << "\"";

        o << data;

        if (quotes)
            o << "\"";

        if (cr)
            o << std::endl;
    }

    void identStr(std::ostream &o, const std::string &s, ui32 data, bool dechex)
    {
        identStr(o, s);
        if (dechex)
            decHexDisplay(o, data);
        else
            o << data;
        o << std::endl;
    }

    void identStr(std::ostream &o, STRID s)
    {
        identStr(o, outstr[s]);
    }

    void identStr(std::ostream &o, STRID s, ui32 data, bool dechex)
    {
        identStr(o, outstr[s], data, dechex);
    }

    void identStr(std::ostream &o, STRID s, const std::string &data, bool cr)
    {
        if (data.length()) {
            identStr(o, outstr[s], data, cr);
        }
    }

    void identStr(std::ostream &o, STRID s, const LanguageCode& lc, bool cr)
    {
        identStr(o, outstr[s], lc.str(), cr);
    }

    void identStr(std::ostream &o, STRID s, const std::vector<ui8> &data, bool cr)
    {
        if (data.size() > 0)
        {
            std::stringstream data_str;

            data_str.unsetf(std::ios::showbase);
            data_str.setf(std::ios::right);
            data_str << std::setfill('0');

            for (ui32 i = 0; i < data.size(); i++)
            {
                data_str << std::setw(2) << std::hex << (ui16) data[i];
            }

            identStr(o, outstr[s], data_str.str(), cr, false);
        }
    }

#endif

} // namespace
