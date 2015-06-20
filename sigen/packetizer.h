//
// packetizer.h: definition of mpeg packetizer class
// -----------------------------------

#ifndef SIGEN_PACKETIZER_H
#define SIGEN_PACKETIZER_H

#include <string>
#include <sigen/types.h>

namespace sigen {

    class Section;

    //
    // mpeg packetizer class
    //
    class MpgPacketizer
    {
    private:
        enum { SYNC_BYTE = 0x47,
               HEADER_SIZE = 4,
               PKT_DATA_SIZE = 184,
               PACKET_SIZE = PKT_DATA_SIZE + 4,
        };

        // data
        std::string filename;

        bool transport_error_indicator,
            transport_priority;
        ui8 continuity_count,
            transport_scrambling_control : 2,
            adaptation_field_control : 2;

        // prohibit
        MpgPacketizer();
        MpgPacketizer(const MpgPacketizer &);
        MpgPacketizer &operator=(const MpgPacketizer &);

    public:
        enum ScramblingControl_t { NOT_SCRAMBLED = 0x00 }; // 0x01-0x11 user def'd
        enum AdaptationField_t {
            RESERVED                     = 0x0,
            NO_ADAPTATION_FIELD          = 0x1, // default
            ADAPTATION_FIELD_ONLY        = 0x2,
            ADAPTATION_FIELD_AND_PAYLOAD = 0x4
        };

        // constructor
        MpgPacketizer(const std::string &out_file, ui8 cont_count);

        void setFlags(bool transport_err_ind, bool transport_pri) {
            transport_error_indicator = transport_err_ind;
            transport_priority = transport_pri;
        }

        int packetize(const Section &section, ui16 pid);

    protected:
        void getHeader(ui8 *packet, const ui8 *section_data,
                       bool payload_unit_start_indicator, ui16 pid);
    };

} // sigen namespace

#endif
