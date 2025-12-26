#include <catch2/catch_test_macros.hpp>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/type_hash/type_name.h"
#endif

TEST_CASE("type_hash<int>", "[type_hash]") {
    CHECK(cista::type_str<int>() == "int");
}