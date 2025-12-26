#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/mutable_fws_multimap.h"
#include "cista/reflection/comparable.h"
#include "cista/verify.h"
#endif

#include <algorithm>
#include <iterator>
#include <vector>

template <typename K, typename V>
using mutable_fws_multimap = cista::offset::mutable_fws_multimap<K, V>;

template <typename T>
using strong_key = cista::strong<T, struct strong_key_tag>;

template <typename T>
using weak_key = T;

namespace utl {

template <typename Container, typename Element>
void erase(Container& c, Element elem) {
  c.erase(std::remove(std::begin(c), std::end(c), elem), std::end(c));
}

template <typename Container, typename Element>
void erase(Container&& c, Element elem) {
  using std::begin;
  using std::end;
  c.erase(std::remove(begin(c), end(c), elem), end(c));
}

template <typename Container, typename Predicate>
void erase_if(Container&& c, Predicate&& pred) {
  using std::begin;
  using std::end;
  c.erase(std::remove_if(begin(c), end(c), std::forward<Predicate>(pred)),
          end(c));
}

}  // namespace utl

struct test_edge {
  CISTA_COMPARABLE()

  std::uint32_t from_{};
  std::uint32_t to_{};
  std::uint32_t weight_{};
};

// Helper function adapted for GTest
template <typename T1, typename T2>
void CheckElementsAreArray(T1 const& t1, std::initializer_list<T2> t2) {
  ASSERT_EQ(t1.size(), t2.size());
  auto it1 = std::begin(t1);
  auto it2 = std::begin(t2);
  auto end1 = std::end(t1);
  auto end2 = std::end(t2);
  for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

std::ostream& operator<<(std::ostream& out, test_edge const& e) {
  return out << "{from=" << e.from_ << ", to=" << e.to_
             << ", weight=" << e.weight_ << "}";
}

template <typename KeyType = unsigned>
mutable_fws_multimap<KeyType, int> build_test_map_1() {
  mutable_fws_multimap<KeyType, int> mm;

  mm[KeyType{0}].push_back(4);
  mm[KeyType{0}].push_back(8);

  mm[KeyType{1}].push_back(15);
  mm[KeyType{1}].push_back(16);
  mm[KeyType{1}].push_back(23);
  mm[KeyType{1}].push_back(42);

  mm[KeyType{2}].push_back(100);
  mm[KeyType{2}].push_back(200);
  mm[KeyType{2}].push_back(250);
  mm[KeyType{2}].push_back(300);
  mm[KeyType{2}].push_back(350);
  mm[KeyType{2}].push_back(400);

  return mm;
}

// --- Template Test Setup ---
template <typename T>
class MutableFwsMultimapTest : public testing::Test {};

using KeyTypes = ::testing::Types<strong_key<unsigned>, weak_key<unsigned>>;
TYPED_TEST_SUITE(MutableFwsMultimapTest, KeyTypes);

TYPED_TEST(MutableFwsMultimapTest, Int1) {
  using KeyType = TypeParam;
  mutable_fws_multimap<KeyType, int> mm;

  ASSERT_EQ(0, mm.element_count());
  ASSERT_EQ(0, mm.size());

  mm[KeyType{0}].push_back(42);
  ASSERT_EQ(1, mm.element_count());
  ASSERT_EQ(1, mm.size());
  CheckElementsAreArray(mm[KeyType{0}], {42});
  EXPECT_EQ(1, mm[KeyType{0}].size());

  mm[KeyType{1}].push_back(4);
  ASSERT_EQ(2, mm.element_count());
  ASSERT_EQ(2, mm.size());
  CheckElementsAreArray(mm[KeyType{0}], {42});
  EXPECT_EQ(1, mm[KeyType{0}].size());
  CheckElementsAreArray(mm[KeyType{1}], {4});
  EXPECT_EQ(1, mm[KeyType{1}].size());

  mm[KeyType{1}].push_back(8);
  ASSERT_EQ(3, mm.element_count());
  ASSERT_EQ(2, mm.size());
  CheckElementsAreArray(mm[KeyType{0}], {42});
  EXPECT_EQ(1, mm[KeyType{0}].size());
  CheckElementsAreArray(mm[KeyType{1}], {4, 8});
  EXPECT_EQ(2, mm[KeyType{1}].size());

  mm[KeyType{1}].emplace_back(15);
  ASSERT_EQ(4, mm.element_count());
  ASSERT_EQ(2, mm.size());
  CheckElementsAreArray(mm[KeyType{0}], {42});
  EXPECT_EQ(1, mm[KeyType{0}].size());
  CheckElementsAreArray(mm[KeyType{1}], {4, 8, 15});
  EXPECT_EQ(3, mm[KeyType{1}].size());

  mm[KeyType{1}].push_back(16);
  ASSERT_EQ(5, mm.element_count());
  ASSERT_EQ(2, mm.size());
  CheckElementsAreArray(mm[KeyType{0}], {42});
  EXPECT_EQ(1, mm[KeyType{0}].size());
  CheckElementsAreArray(mm[KeyType{1}], {4, 8, 15, 16});
  EXPECT_EQ(4, mm[KeyType{1}].size());

  mm[KeyType{0}].push_back(23);
  ASSERT_EQ(6, mm.element_count());
  ASSERT_EQ(2, mm.size());
  CheckElementsAreArray(mm[KeyType{0}], {42, 23});
  EXPECT_EQ(2, mm[KeyType{0}].size());
  CheckElementsAreArray(mm[KeyType{1}], {4, 8, 15, 16});
  EXPECT_EQ(4, mm[KeyType{1}].size());
}

TEST(MutableFwsMultimapTest, Graph1) {
  mutable_fws_multimap<unsigned, test_edge> mm;

  mm[0].emplace_back(0U, 1U, 10U);
  mm[1].emplace_back(1U, 2U, 20U);
  mm[1].emplace_back(1U, 3U, 30U);
  mm[3].emplace_back(3U, 0U, 50U);
  mm[2].emplace_back(2U, 3U, 5U);

  ASSERT_EQ(4, mm.size());
  EXPECT_EQ(5, mm.element_count());

  CheckElementsAreArray(mm[0], {test_edge{0U, 1U, 10U}});
  CheckElementsAreArray(mm[1],
                         {test_edge{1U, 2U, 20U}, test_edge{1U, 3U, 30U}});
  CheckElementsAreArray(mm[2], {test_edge{2U, 3U, 5U}});
  CheckElementsAreArray(mm[3], {test_edge{3U, 0U, 50U}});
}

TEST(MutableFwsMultimapTest, Int2) {
  auto const mm = build_test_map_1();

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(12, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8});
  CheckElementsAreArray(mm[1], {15, 16, 23, 42});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350, 400});

  CheckElementsAreArray(mm.front(), {4, 8});
  CheckElementsAreArray(mm.back(), {100, 200, 250, 300, 350, 400});
  EXPECT_EQ(15, mm[1].front());
  EXPECT_EQ(42, mm[1].back());
}

TYPED_TEST(MutableFwsMultimapTest, IntInsert1) {
  using KeyType = TypeParam;
  auto mm = build_test_map_1<KeyType>();

  mm[KeyType{1}].insert(std::next(mm[KeyType{1}].begin(), 2), 20);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(13, mm.element_count());
  CheckElementsAreArray(mm[KeyType{0}], {4, 8});
  CheckElementsAreArray(mm[KeyType{1}], {15, 16, 20, 23, 42});
  CheckElementsAreArray(mm[KeyType{2}], {100, 200, 250, 300, 350, 400});
}

TEST(MutableFwsMultimapTest, IntInsert2) {
  auto mm = build_test_map_1();

  auto const val = 20;
  mm[1].insert(std::next(mm[1].begin(), 2), val);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(13, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8});
  CheckElementsAreArray(mm[1], {15, 16, 20, 23, 42});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350, 400});
}

TYPED_TEST(MutableFwsMultimapTest, IntErase1) {
  using KeyType = TypeParam;
  auto mm = build_test_map_1<KeyType>();

  utl::erase(mm[KeyType{1}], 16);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(11, mm.element_count());
  CheckElementsAreArray(mm[KeyType{0}], {4, 8});
  CheckElementsAreArray(mm[KeyType{1}], {15, 23, 42});
  CheckElementsAreArray(mm[KeyType{2}], {100, 200, 250, 300, 350, 400});

  utl::erase(mm[KeyType{2}], 100);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(10, mm.element_count());
  CheckElementsAreArray(mm[KeyType{0}], {4, 8});
  CheckElementsAreArray(mm[KeyType{1}], {15, 23, 42});
  CheckElementsAreArray(mm[KeyType{2}], {200, 250, 300, 350, 400});

  utl::erase(mm[KeyType{2}], 400);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(9, mm.element_count());
  CheckElementsAreArray(mm[KeyType{0}], {4, 8});
  CheckElementsAreArray(mm[KeyType{1}], {15, 23, 42});
  CheckElementsAreArray(mm[KeyType{2}], {200, 250, 300, 350});

  utl::erase(mm[KeyType{2}], 250);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(8, mm.element_count());
  CheckElementsAreArray(mm[KeyType{0}], {4, 8});
  CheckElementsAreArray(mm[KeyType{1}], {15, 23, 42});
  CheckElementsAreArray(mm[KeyType{2}], {200, 300, 350});

  utl::erase(mm[KeyType{1}], 404);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(8, mm.element_count());
  CheckElementsAreArray(mm[KeyType{0}], {4, 8});
  CheckElementsAreArray(mm[KeyType{1}], {15, 23, 42});
  CheckElementsAreArray(mm[KeyType{2}], {200, 300, 350});
}

TEST(MutableFwsMultimapTest, IntErase2) {
  auto mm = build_test_map_1();

  utl::erase_if(mm[2], [](int e) { return e % 100 == 0; });

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(8, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8});
  CheckElementsAreArray(mm[1], {15, 16, 23, 42});
  CheckElementsAreArray(mm[2], {250, 350});
}

TEST(MutableFwsMultimapTest, IntResize1) {
  auto mm = build_test_map_1();

  mm[0].resize(4);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(14, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8, 0, 0});
  CheckElementsAreArray(mm[1], {15, 16, 23, 42});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350, 400});

  mm[1].resize(3);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(13, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8, 0, 0});
  CheckElementsAreArray(mm[1], {15, 16, 23});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350, 400});

  mm[1].resize(6, 123);

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(16, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8, 0, 0});
  CheckElementsAreArray(mm[1], {15, 16, 23, 123, 123, 123});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350, 400});
}

TEST(MutableFwsMultimapTest, PopBack1) {
  auto mm = build_test_map_1();

  mm[2].pop_back();

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(11, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8});
  CheckElementsAreArray(mm[1], {15, 16, 23, 42});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350});

  mm[1].pop_back();

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(10, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8});
  CheckElementsAreArray(mm[1], {15, 16, 23});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350});

  mm[0].pop_back();

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(9, mm.element_count());
  CheckElementsAreArray(mm[0], {4});
  CheckElementsAreArray(mm[1], {15, 16, 23});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350});

  mm[0].pop_back();

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(8, mm.element_count());
  EXPECT_TRUE(mm[0].empty());
  CheckElementsAreArray(mm[1], {15, 16, 23});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350});
}

TEST(MutableFwsMultimapTest, Clear1) {
  auto mm = build_test_map_1();

  mm[0].clear();

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(10, mm.element_count());
  EXPECT_TRUE(mm[0].empty());
  CheckElementsAreArray(mm[1], {15, 16, 23, 42});
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350, 400});
}

TEST(MutableFwsMultimapTest, Clear2) {
  auto mm = build_test_map_1();

  mm[1].clear();

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(8, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8});
  EXPECT_TRUE(mm[1].empty());
  CheckElementsAreArray(mm[2], {100, 200, 250, 300, 350, 400});
}

TEST(MutableFwsMultimapTest, Clear3) {
  auto mm = build_test_map_1();

  mm[2].clear();

  ASSERT_EQ(3, mm.size());
  EXPECT_EQ(6, mm.element_count());
  CheckElementsAreArray(mm[0], {4, 8});
  CheckElementsAreArray(mm[1], {15, 16, 23, 42});
  EXPECT_TRUE(mm[2].empty());
}

template <typename T>
T get_order_loop(T const size) {
  for (auto order = T{0U}, value = T{1}; order < static_cast<T>(sizeof(T) * 8);
       ++order, value = value << 1) {
    if (value == size) {
      return order;
    }
  }
  cista::verify(false, "mutable_fws_multimap::get_order: not found for");
  return T{};
}

TEST(MutableFwsMultimapTest, GetOrderLoop) {
  for (std::uint16_t i = 0U; i < 16; ++i) {
    EXPECT_EQ(get_order_loop(static_cast<std::uint16_t>(1U) << i), i);
  }

  for (std::uint32_t i = 0U; i < 32; ++i) {
    EXPECT_EQ(get_order_loop(1U << i), i);
  }

  for (std::uint64_t i = 0ULL; i < 64; ++i) {
    EXPECT_EQ(get_order_loop(1ULL << i), i);
  }
}
