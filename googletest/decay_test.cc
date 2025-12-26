#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/decay.h"
#include "cista/reflection/for_each_field.h"
#endif

using cista::decay_t;

static_assert(std::is_same_v<decay_t<std::reference_wrapper<int const>>, int>);
static_assert(std::is_same_v<decay_t<std::reference_wrapper<int const&>>, int>);
static_assert(
    std::is_same_v<decay_t<std::reference_wrapper<int const&&>>, int>);
static_assert(
    std::is_same_v<decay_t<std::reference_wrapper<int const&&>&>, int>);
static_assert(std::is_same_v<decay_t<int const>, int>);
static_assert(std::is_same_v<decay_t<int const&>, int>);

void test(int const& i) {
  static_assert(std::is_same_v<decay_t<decltype(i)>, int>);
}

TEST(DecayTest, ReferenceWrapperField) {
  int i;

  struct a {
    explicit a(int& i) : i_{i} {}
    std::reference_wrapper<int> i_;
  } instance{i};

  cista::for_each_field(instance, [](auto&& f) { f.get() = 7; });

  EXPECT_EQ(i, 7);
}

template <typename T>
auto decay_type(T& type) {
  using DecayedType = decay_t<decltype(type)>;
  return DecayedType(type);
}

TEST(DecayTest, Decay) {
  struct a {
    int number = 42;
  } instance;

  auto& simple = instance;
  auto wrapper = std::reference_wrapper<a>(instance);

  auto result1 = decay_type(instance);
  auto result2 = decay_type(simple);
  auto result3 = decay_type(wrapper);

  instance.number = 0;

  EXPECT_EQ(result1.number, 42);
  EXPECT_EQ(result2.number, 42);
  EXPECT_EQ(result3.number, 42);
}
