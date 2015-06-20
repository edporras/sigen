//
// tstream.cc: definitions for the transport stream object
// and transport stream section object.
// -----------------------------------

#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <string>
#include <list>
#include "dump.h"
#include "tstream.h"
#include "util.h"

namespace sigen
{
    //
    // crc32
    namespace tstream_priv {
        //    const ui32 POLYNOMIAL = 0x04c11db7L;

        const int MAX_CRC_ENTRIES = 256;

        // crc table data
        const ui32 CrcTable[ MAX_CRC_ENTRIES ] = {
                     0U,   79764919U,  159529838U,  222504665U,
             319059676U,  398814059U,  445009330U,  507990021U,
             638119352U,  583659535U,  797628118U,  726387553U,
             890018660U,  835552979U, 1015980042U,  944750013U,
            1276238704U, 1221641927U, 1167319070U, 1095957929U,
            1595256236U, 1540665371U, 1452775106U, 1381403509U,
            1780037320U, 1859660671U, 1671105958U, 1733955601U,
            2031960084U, 2111593891U, 1889500026U, 1952343757U,
            2552477408U, 2632100695U, 2443283854U, 2506133561U,
            2334638140U, 2414271883U, 2191915858U, 2254759653U,
            3190512472U, 3135915759U, 3081330742U, 3009969537U,
            2905550212U, 2850959411U, 2762807018U, 2691435357U,
            3560074640U, 3505614887U, 3719321342U, 3648080713U,
            3342211916U, 3287746299U, 3467911202U, 3396681109U,
            4063920168U, 4143685023U, 4223187782U, 4286162673U,
            3779000052U, 3858754371U, 3904687514U, 3967668269U,
             881225847U,  809987520U, 1023691545U,  969234094U,
             662832811U,  591600412U,  771767749U,  717299826U,
             311336399U,  374308984U,  453813921U,  533576470U,
              25881363U,   88864420U,  134795389U,  214552010U,
            2023205639U, 2086057648U, 1897238633U, 1976864222U,
            1804852699U, 1867694188U, 1645340341U, 1724971778U,
            1587496639U, 1516133128U, 1461550545U, 1406951526U,
            1302016099U, 1230646740U, 1142491917U, 1087903418U,
            2896545431U, 2825181984U, 2770861561U, 2716262478U,
            3215044683U, 3143675388U, 3055782693U, 3001194130U,
            2326604591U, 2389456536U, 2200899649U, 2280525302U,
            2578013683U, 2640855108U, 2418763421U, 2498394922U,
            3769900519U, 3832873040U, 3912640137U, 3992402750U,
            4088425275U, 4151408268U, 4197601365U, 4277358050U,
            3334271071U, 3263032808U, 3476998961U, 3422541446U,
            3585640067U, 3514407732U, 3694837229U, 3640369242U,
            1762451694U, 1842216281U, 1619975040U, 1682949687U,
            2047383090U, 2127137669U, 1938468188U, 2001449195U,
            1325665622U, 1271206113U, 1183200824U, 1111960463U,
            1543535498U, 1489069629U, 1434599652U, 1363369299U,
             622672798U,  568075817U,  748617968U,  677256519U,
             907627842U,  853037301U, 1067152940U,  995781531U,
              51762726U,  131386257U,  177728840U,  240578815U,
             269590778U,  349224269U,  429104020U,  491947555U,
            4046411278U, 4126034873U, 4172115296U, 4234965207U,
            3794477266U, 3874110821U, 3953728444U, 4016571915U,
            3609705398U, 3555108353U, 3735388376U, 3664026991U,
            3290680682U, 3236090077U, 3449943556U, 3378572211U,
            3174993278U, 3120533705U, 3032266256U, 2961025959U,
            2923101090U, 2868635157U, 2813903052U, 2742672763U,
            2604032198U, 2683796849U, 2461293480U, 2524268063U,
            2284983834U, 2364738477U, 2175806836U, 2238787779U,
            1569362073U, 1498123566U, 1409854455U, 1355396672U,
            1317987909U, 1246755826U, 1192025387U, 1137557660U,
            2072149281U, 2135122070U, 1912620623U, 1992383480U,
            1753615357U, 1816598090U, 1627664531U, 1707420964U,
             295390185U,  358241886U,  404320391U,  483945776U,
              43990325U,  106832002U,  186451547U,  266083308U,
             932423249U,  861060070U, 1041341759U,  986742920U,
             613929101U,  542559546U,  756411363U,  701822548U,
            3316196985U, 3244833742U, 3425377559U, 3370778784U,
            3601682597U, 3530312978U, 3744426955U, 3689838204U,
            3819031489U, 3881883254U, 3928223919U, 4007849240U,
            4037393693U, 4100235434U, 4180117107U, 4259748804U,
            2310601993U, 2373574846U, 2151335527U, 2231098320U,
            2596047829U, 2659030626U, 2470359227U, 2550115596U,
            2947551409U, 2876312838U, 2788305887U, 2733848168U,
            3165939309U, 3094707162U, 3040238851U, 2985771188U,
        };
    }

    using namespace tstream_priv;

    // --------------------------------
    // dvb section class
    //
    Section::Section(ui16 s) :
        crc(0), data_length(0), size(s)
    {
        data = new ui8[s];
        memset(data, 0xff, s);
        pos = data;
    }

    // data copiers
    //
    bool Section::set08Bits(ui8 d)
    {
        assert( lengthFits(sizeof(d)) );

        *pos++ = d;
        data_length += sizeof(d);
        return true;
    }

    bool Section::set16Bits(ui16 d)
    {
        assert( lengthFits(sizeof(d)) );

        *pos++ = (d >> 8) & 0x00ff;
        *pos++ = d & 0x00ff;
        data_length += sizeof(d);
        return true;
    }

    bool Section::set24Bits(const ui8 *d)
    {
        assert( lengthFits(3) );

        *pos++ = d[0];
        *pos++ = d[1];
        *pos++ = d[2];
        data_length += 3;
        return true;
    }

    bool Section::set24Bits(const ui32 d)
    {
        assert( lengthFits(3) );

        // copy the lower 24 bits
        *pos++ = (d >> 16) & 0x000000ff;
        *pos++ = (d >> 8) & 0x000000ff;
        *pos++ = d & 0x000000ff;
        data_length += 3;
        return true;
    }

    bool Section::set32Bits(ui32 d)
    {
        assert( lengthFits(sizeof(d)) );

        *pos++ = ((ui8) (d >> 24)) & 0xff;
        *pos++ = ((ui8) (d >> 16)) & 0xff;
        *pos++ = ((ui8) (d >> 8)) & 0xff;
        *pos++ = ((ui8) d) & 0xff;
        data_length += sizeof(d);
        return true;
    }

    //
    // writes a string of data
    bool Section::setBits(const std::string &data) {
        int len = data.length();

        assert( lengthFits(len) );

        for (int i = 0; i < len; i++)
            set08Bits(data[i]);

        // don't add length as it is done by set08Bits()
        return true;
    }

    //
    // writes the data of a language code
    bool Section::setBits(const LanguageCode &code)
    {
        return setBits(code.str());
    }

    //
    // writes a sequency of bytes in vector form
    bool Section::setBits(const std::vector<ui8> &data) {
        int len = data.size();

        assert( lengthFits(len) );

        for (int i = 0; i < len; i++)
            set08Bits(data[i]);

        // don't add length as it is done by set08Bits()
        return true;
    }

    //
    // these don't increment the cur position
    bool Section::set08Bits(ui8 idx, ui8 d)
    {
        if (idx > size)
        {
            std::cerr << "Section::set08Bits(ui8, ui8): invalid index.. set aborted" << std::endl;
            return false;
        }
        data[idx] = d;
        return true;
    }

    bool Section::set16Bits(ui8 *p, ui16 d)
    {
        *p++ = (d >> 8) & 0x00ff;
        *p = d & 0x00ff;
        return true;
    }

    bool Section::set16Bits(ui16 idx, ui16 d)
    {
        if (idx + 1 > size)
        {
            std::cerr << "Section::set16Bits(ui16, ui16): invalid index.. set aborted" << std::endl;
            return false;
        }
        data[idx] = (d >> 8) & 0x00ff;
        data[idx + 1] = d & 0x00ff;
        return true;
    }


    //
    // write the buffer to a file
    //
    void Section::write(std::ostream &o) const
    {
        for (int i = 0; i < data_length; i++)
            o << data[i];
    }

    //
    // add a crc at the end of the data
    //
    bool Section::calcCrc()
    {
        ui8 *d = &data[0];
        int i;
        ui32 crc_accum = 0xffffffff;

        assert( lengthFits(CRC_LEN) );

        for ( int j = 0;  j < data_length;  j++ ) {
            i = ( (int) ( crc_accum >> 24) ^ *d++ ) & 0xff;
            crc_accum = ( crc_accum << 8 ) ^ CrcTable[i];
        }
        crc = crc_accum;
        set32Bits(crc);
        return true;
    }

    //
    // display the section in binary / char mode
    //
#ifdef ENABLE_DUMP
    inline void Section::dump(std::ostream &o) const
    {
        dumpData(o, data, data_length);
    }
#endif


    // --------------------------------
    // TStream class
    // constructor / destructor

    TStream::~TStream()
    {
        // delete any allocated sections
        std::list<Section *>::const_iterator s_iter = section_list.begin();

        while (s_iter != section_list.end())
            delete *s_iter++;
    }


    //
    // allocates a new section
    //
    Section *TStream::getNewSection(ui16 size)
    {
        Section *sec = new Section( size );
        section_list.push_back( sec );
        return sec;
    }


    //
    // dump to the file
    //
    void TStream::write(const std::string &file_name) const
    {
        std::stringstream strm;
        std::list<Section *>::const_iterator s_iter = section_list.begin();

        // dump all sections to the stream in memory
        while (s_iter != section_list.end())
        {
            const Section *s = *s_iter++;
            s->write(strm);
        }

        // open the file and save the contents of the stream to it
        std::ofstream f( file_name.c_str() );

        while (strm.peek() != EOF)
            f.put(strm.get());
    }


    //
    // to stdout
    //
#ifdef ENABLE_DUMP
    void TStream::dump(std::ostream &o) const
    {
        std::ios::fmtflags f = o.flags();

        o << std::endl
          << "-- Section dump - num_sections: "
          << std::dec << section_list.size() << " --" << std::endl
          << std::endl;

        int cnt = 0;
        std::list<Section *>::const_iterator s_iter = section_list.begin();

        // dump all sections
        while (s_iter != section_list.end())
        {
            const Section *s = *s_iter++;

            o << "- sec: " << std::dec << cnt++
              << ", length: " << std::dec << s->length()
              << ", size (max): " << std::dec << s->capacity()
              << ", data: " << std::endl;
            s->dump(o);
        }

        o.flags( f );
    }
#endif

} // namespace
