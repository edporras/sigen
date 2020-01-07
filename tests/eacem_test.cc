#include "../src/sigen.h"
#include "dvb_builder.h"

using namespace sigen;

namespace tests
{
   int eacem(TStream& t)
   {
      // EACEM tests
      NITOther nit(0x1234, 1);

      // Stream Identifier Desc
      nit.addNetworkDesc(*(new EACEM::StreamIdentifierDesc));

      // EACEM Private Data Specifier Desc (with EACEM PDSD)
      nit.addNetworkDesc(*(new EACEM::PrivateDataSpecifierDesc));

      // Logical Channel Number Desc
      auto* lcnd = new EACEM::LogicalChannelDesc;
      lcnd->addLogicalChan(0x1000, 0x10);
      lcnd->addLogicalChan(0x1001, 0x11, false);
      lcnd->addLogicalChan(0x1002, 0x12);
      lcnd->addLogicalChan(0x1003); // non-selectable service (LCN = 0)

      nit.addNetworkDesc(*lcnd);

      // build sections
      DUMP(nit);
      nit.buildSections(t);

      // dump built sections
      DUMP(t);

      return tests::cmp_bin(t, "reference/eacem.ts");
   }
}
