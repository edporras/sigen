#pragma once

#include <string>
#include "../src/sigen.h"

namespace tests {
   int pat(sigen::TStream& t);
   int pmt(sigen::TStream& t);
   int nit(sigen::TStream& t);
   int sdt(sigen::TStream& t);
   int bat(sigen::TStream& t);
   int eit(sigen::TStream& t);
   int tot(sigen::TStream& t);
   int cat(sigen::TStream& t);
   int tdt(sigen::TStream& t);
   int rst(sigen::TStream& t);
   int st(sigen::TStream& t);
   int eacem(sigen::TStream& t);
   int other(sigen::TStream& t);

   int cmp_bin(const sigen::TStream& ts, const std::string& filename);
   bool write_bin(const sigen::TStream& ts, const std::string& basename);
}
