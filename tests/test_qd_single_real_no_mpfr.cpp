#include <qd/ds_real.h>
#include <qd/qs_real.h>
#include <qd/td_real.h>
#include <qd/ts_real.h>
#include <qd/qd_config.h>
#if defined(QD_HAVE_EDD_REAL) && QD_HAVE_EDD_REAL
#  include <qd/edd_real.h>
#endif

#include <cmath>

int main()
{
    const ds_real ds = ds_real(1.0f) +
                       ds_real(std::ldexp(1.0f, -20));
    const ts_real ts = ts_real(1.0f) +
                       ts_real(std::ldexp(1.0f, -20),
                               std::ldexp(1.0f, -40));
    const qs_real qs = qs_real(1.0f) +
                       qs_real(std::ldexp(1.0f, -20),
                               std::ldexp(1.0f, -40),
                               std::ldexp(1.0f, -60));
    const td_real td = td_real(1.0) +
                       td_real(std::ldexp(1.0, -50),
                               std::ldexp(1.0, -100),
                               0.0);
    if (!(ds[0] > 1.0f && ts[0] > 1.0f &&
          qs[0] > 1.0f && td[0] > 1.0)) {
        return 1;
    }
#if defined(QD_HAVE_EDD_REAL) && QD_HAVE_EDD_REAL
    const edd_real edd = edd_real(1.0L) + edd_real(std::ldexp(1.0L, -70));
    if (!(edd[0] > 1.0L || edd[1] > 0.0L)) return 1;
#endif
    return 0;
}
