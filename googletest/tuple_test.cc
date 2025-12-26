#include <string>
#include <limits>
#include <array>
#include <gtest/gtest.h>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/serialization.h"
#endif

namespace data = cista::offset;

TEST(TupleTest, Get) {
  auto t = cista::tuple{1, .1, 'a'};
  static_assert(alignof(decltype(t)) == 8);

  cista::get<2>(t) = 'B';
  EXPECT_EQ(cista::get<0>(t), 1);
  EXPECT_DOUBLE_EQ(cista::get<1>(t), 0.1);
  EXPECT_EQ(cista::get<2>(t), 'B');
  EXPECT_EQ(cista::tuple_size_v<decltype(t)>, 3U);
  cista::apply([](auto&... f) { ((f = {}), ...); }, t);
  EXPECT_EQ(cista::get<0>(t), 0);
  EXPECT_DOUBLE_EQ(cista::get<1>(t), 0.0);
  EXPECT_EQ(cista::get<2>(t), '\0');
}

TEST(TupleTest, GetWithSameTypes) {
  auto t = cista::tuple{1, 42, 1336, 11};

  cista::get<2>(t) += 1;

  EXPECT_EQ(cista::get<0>(t), 1);
  EXPECT_EQ(cista::get<1>(t), 42);
  EXPECT_EQ(cista::get<2>(t), 1337);
  EXPECT_EQ(cista::get<3>(t), 11);
  EXPECT_EQ(cista::tuple_size_v<decltype(t)>, 4U);
  cista::apply([](auto&... f) { ((f = {}), ...); }, t);
  EXPECT_EQ(cista::get<0>(t), 0);
  EXPECT_EQ(cista::get<1>(t), 0);
  EXPECT_EQ(cista::get<2>(t), 0);
  EXPECT_EQ(cista::get<3>(t), 0);
}

TEST(TupleTest, StructuredBindings) {
  auto t = cista::tuple{1, 42, 1336, 11};
  auto& [t0, t1, t2, t3] = t;
  t2 += 1;
  EXPECT_EQ(t0, 1);
  EXPECT_EQ(t1, 42);
  EXPECT_EQ(t2, 1337);
  EXPECT_EQ(t3, 11);
}

TEST(TupleTest, Comparison) {
  auto a = cista::tuple{2, 'a'};
  auto b = cista::tuple{1, 'b'};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
  EXPECT_TRUE(b < a);
  EXPECT_TRUE(b <= a);
  EXPECT_TRUE(a > b);
  EXPECT_FALSE(b >= a);
}

TEST(TupleTest, TupleElementT) {
  static_assert(
      std::is_same_v<
          uint32_t, cista::tuple_element_t<
                        0, cista::tuple<uint32_t, uint64_t, float, double>>>);
  static_assert(
      std::is_same_v<
          uint64_t, cista::tuple_element_t<
                        1, cista::tuple<uint32_t, uint64_t, float, double>>>);
  static_assert(
      std::is_same_v<
          float, cista::tuple_element_t<
                     2, cista::tuple<uint32_t, uint64_t, float, double>>>);
  static_assert(
      std::is_same_v<
          double, cista::tuple_element_t<
                      3, cista::tuple<uint32_t, uint64_t, float, double>>>);
}

TEST(TupleTest, SimpleGet) {
  cista::tuple<uint32_t, uint64_t, float, double> t = {42, 1082342034, 0.42F, 0.001337};

  EXPECT_EQ(cista::get<0>(t), 42);
  EXPECT_EQ(cista::get<1>(t), 1082342034);
  EXPECT_FLOAT_EQ(cista::get<2>(t), 0.42F);
  EXPECT_DOUBLE_EQ(cista::get<3>(t), 0.001337);
}

TEST(TupleTest, SimpleGetMax) {
  cista::tuple<uint32_t, uint64_t, float, double> t2 = {
      std::numeric_limits<uint32_t>::max(),
      std::numeric_limits<uint64_t>::max(),
      std::numeric_limits<float>::max(),
      std::numeric_limits<double>::max()};

  EXPECT_EQ(cista::get<0>(t2), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(cista::get<1>(t2), std::numeric_limits<uint64_t>::max());
  EXPECT_FLOAT_EQ(cista::get<2>(t2), std::numeric_limits<float>::max());
  EXPECT_DOUBLE_EQ(cista::get<3>(t2), std::numeric_limits<double>::max());
}

TEST(TupleTest, ContainerGetWithPrecedingType) {
  cista::tuple<std::size_t, data::vector<float>> t = {100, {0.1F, 0.2F, 10.11F}};

  auto& vec = cista::get<1>(t);
  EXPECT_FLOAT_EQ(vec.front(), 0.1F);
  EXPECT_FLOAT_EQ(vec[1], 0.2F);
  EXPECT_FLOAT_EQ(vec.back(), 10.11F);
  EXPECT_EQ(cista::get<0>(t), 100U);
}

TEST(TupleTest, ContainerGetWithSucceedingType) {
  cista::tuple<data::vector<float>, std::size_t> t = {{0.1F, 0.2F, 10.11F}, 100};

  auto& vec = cista::get<0>(t);
  EXPECT_FLOAT_EQ(vec.front(), 0.1F);
  EXPECT_FLOAT_EQ(vec[1], 0.2F);
  EXPECT_FLOAT_EQ(vec.back(), 10.11F);
  EXPECT_EQ(cista::get<1>(t), 100U);
}

TEST(TupleTest, ContainerGetPrecAndSucType) {
  cista::tuple<std::size_t, data::vector<float>, std::size_t> t = {
      10, {0.1F, 0.2F, 10.11F}, 1000};

  EXPECT_EQ(cista::get<0>(t), 10U);
  EXPECT_FLOAT_EQ(cista::get<1>(t).front(), 0.1F);
  EXPECT_FLOAT_EQ(cista::get<1>(t)[1], 0.2F);
  EXPECT_FLOAT_EQ(cista::get<1>(t).back(), 10.11F);
  EXPECT_EQ(cista::get<2>(t), 1000U);
}

TEST(TupleTest, SerializePrimitives) {
  using serialize_me = cista::tuple<uint32_t, uint64_t, float, double>;
  static_assert(alignof(serialize_me) == 8);

  cista::byte_buf buf;
  {
    serialize_me t = {42, 1082342034, 0.42F, 0.001337};
    buf = cista::serialize(t);
  }

  serialize_me d_t = *cista::deserialize<serialize_me>(buf);
  cista::get<0>(d_t) += 1;

  EXPECT_EQ(cista::get<0>(d_t), 43);
  EXPECT_EQ(cista::get<1>(d_t), 1082342034);
  EXPECT_FLOAT_EQ(cista::get<2>(d_t), 0.42F);
  EXPECT_DOUBLE_EQ(cista::get<3>(d_t), 0.001337);
}

TEST(TupleTest, SerializeContainer) {
  cista::byte_buf buf;
  using serialize_me = cista::tuple<std::size_t, data::vector<float>, std::size_t>;

  {
    serialize_me t = {10, {0.1F, 0.2F, 10.11F}, 1000};
    buf = cista::serialize(t);
  }
  auto d_t = *cista::deserialize<serialize_me>(buf);

  EXPECT_EQ(cista::get<0>(d_t), 10U);
  EXPECT_FLOAT_EQ(cista::get<1>(d_t).front(), 0.1F);
  EXPECT_FLOAT_EQ(cista::get<1>(d_t)[1], 0.2F);
  EXPECT_FLOAT_EQ(cista::get<1>(d_t).back(), 10.11F);
  EXPECT_EQ(cista::get<2>(d_t), 1000U);
}

TEST(TupleTest, SerializeStructWithTuple) {
  struct s {
    cista::tuple<int, data::vector<uint32_t>, int> t_;
  };

  cista::byte_buf buf;
  {
    s obj;
    obj.t_ = {10, {1, 2, 3}, 1000};
    buf = cista::serialize(obj);
  }

  auto d_s = *cista::deserialize<s>(buf);
  cista::get<1>(d_s.t_).push_back(4);

  EXPECT_EQ(cista::get<0>(d_s.t_), 10);
  EXPECT_EQ(cista::get<1>(d_s.t_).front(), 1U);
  EXPECT_EQ(cista::get<1>(d_s.t_)[1], 2U);
  EXPECT_EQ(cista::get<1>(d_s.t_)[2], 3U);
  EXPECT_EQ(cista::get<1>(d_s.t_).back(), 4U);
  EXPECT_EQ(cista::get<2>(d_s.t_), 1000);
}

TEST(TupleTest, TupleSerialize) {
  namespace data = cista::offset;
  using serialize_me = cista::tuple<
      data::vector<cista::tuple<data::string, data::hash_set<data::string>>>,
      int>;
  constexpr auto MODE = cista::mode::WITH_VERSION;

  cista::byte_buf buf;
  {
    serialize_me obj;
    cista::get<0>(obj).emplace_back(
        cista::tuple{data::string{"hello"},
                     data::hash_set<data::string>{
                         data::string{"1"},
                         data::string{"this is a very very very long string"},
                         data::string{"3"}}});
    cista::get<0>(obj).emplace_back(
        cista::tuple{data::string{"world"},
                     data::hash_set<data::string>{
                         data::string{"4"},
                         data::string{"this is a very very very long string"},
                         data::string{"6"}}});
    cista::get<1>(obj) = 55;
    buf = cista::serialize<MODE>(obj);
  }

  auto const& serialized = *cista::unchecked_deserialize<serialize_me, MODE>(buf);
  EXPECT_EQ(
      cista::get<0>(serialized).at(0),
      (cista::tuple{data::string{"hello"},
                    data::hash_set<data::string>{
                        data::string{"1"},
                        data::string{"this is a very very very long string"},
                        data::string{"3"}}}));
  EXPECT_EQ(
      cista::get<0>(serialized).at(1),
      (cista::tuple{data::string{"world"},
                    data::hash_set<data::string>{
                        data::string{"4"},
                        data::string{"this is a very very very long string"},
                        data::string{"6"}}}));
  EXPECT_EQ(cista::get<1>(serialized), 55);
}

#ifndef _WIN32
TEST(TupleTest, SizeCheck) {
  using pixbuf = std::array<uint32_t, 1920 * 1080>;
  enum class error_code { success, fail };

  auto check_size = []<typename... Ts>() {
    EXPECT_EQ(sizeof(std::tuple<Ts...>), sizeof(cista::tuple<Ts...>));
  };

  check_size.operator()<pixbuf, error_code>();
  check_size.operator()<uint16_t, uint32_t, uint64_t>();
  check_size.operator()<uint32_t, uint64_t, uint16_t>();
  check_size.operator()<uint16_t, uint32_t, uint16_t, uint64_t>();
  check_size.operator()<uint16_t, uint32_t, uint16_t, uint16_t, uint64_t>();
  check_size.operator()<uint16_t, uint32_t, uint8_t, uint8_t, uint8_t, uint64_t>();
  check_size.operator()<uint64_t, uint32_t, float, uint8_t, uint8_t, bool, double,
                        uint64_t, uint8_t>();
}
#endif