#include <gmpfrxx_mkII.h>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#ifndef GMPFRXX_MKII_SOURCE_DIR
#define GMPFRXX_MKII_SOURCE_DIR "."
#endif

namespace {

std::string read_file(const char* path)
{
    std::ifstream in(path);
    if (!in) {
        std::abort();
    }
    std::ostringstream out;
    out << in.rdbuf();
    return out.str();
}

void require_absent(const std::string& text, const char* needle)
{
    if (text.find(needle) != std::string::npos) {
        std::abort();
    }
}

} // namespace

int main()
{
    const std::string source_dir = GMPFRXX_MKII_SOURCE_DIR;
    const std::string gmp_header_path = source_dir + "/include/gmpxx_mkII.h";
    const std::string mpfr_header_path = source_dir + "/include/mpfrxx_mkII.h";

    const std::string gmp_header = read_file(gmp_header_path.c_str());
    require_absent(gmp_header, "#include <mpfr.h>");
    require_absent(gmp_header, "#include <mpc.h>");
    require_absent(gmp_header, "mpfr_");
    require_absent(gmp_header, "mpc_");
    require_absent(gmp_header, "mpc_t");

    const std::string mpfr_header = read_file(mpfr_header_path.c_str());
    if (mpfr_header.find("#include <mpc.h>") == std::string::npos) {
        std::abort();
    }
    require_absent(mpfr_header, "#include <gmpxx_mkII.h>");
    require_absent(mpfr_header, "mpf_impl.hpp");
    require_absent(mpfr_header, "mpfc_impl.hpp");

    return 0;
}
