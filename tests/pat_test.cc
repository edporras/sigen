#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int pat(TStream& t)
   {
      // build the pat object..
      PAT pat( 0x10, 0x01 );

      // override the default maximum section length.. (optional)
      pat.setMaxSectionLen( 900 );

      // add the services (programs).. first, set the network pid (required for
      // some systems)
      pat.addNetworkPid( 0x20 );

      for (int i = 0; i < 10; i++)
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

      // dump built sections
      DUMP(t);

      return cmp_bin(t, "reference/pat.ts");
   }
}
