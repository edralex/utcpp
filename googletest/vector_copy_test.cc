#include <gtest/gtest.h>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/vector.h"
#endif

TEST(VectorTest, ToTuple) {
  namespace data = cista::offset;
  data::vector<uint32_t> a;
  a.emplace_back(1u);
  a.emplace_back(2u);
  a.emplace_back(3u);

  data::vector<uint32_t> b;
  b = a;

  EXPECT_EQ(a, b);
}