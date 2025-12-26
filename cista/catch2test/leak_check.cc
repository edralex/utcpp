#include <catch2/catch_test_macros.hpp>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/hash_map.h"
#include "cista/containers/string.h"
#endif

namespace data = cista::offset;

TEST_CASE("leak_check", "[containers]") {
    data::hash_map<data::string, uint8_t> m{{"longlonglonglonglonglonglong", 0},
                                            {"short", 0}};
}