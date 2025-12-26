#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/type_hash/type_name.h"
#endif

TEST(TypeHashTest, TypeNameInt) {
    EXPECT_EQ(cista::type_str<int>(), "int");
}