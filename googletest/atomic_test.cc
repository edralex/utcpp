#include <gtest/gtest.h>

#include <thread>
#include <vector>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/atomic.h"
#endif

unsigned long get_random_number() {  // period 2^96-1
  static std::uint64_t x = 123456789, y = 362436069, z = 521288629;

  unsigned long t;
  x ^= x << 16;
  x ^= x >> 5;
  x ^= x << 1;

  t = x;
  x = y;
  y = z;
  z = t ^ x ^ y;

  return z;
}

TEST(AtomicTest, MinTest) {
  constexpr auto const kNValues = 1'000'000;
  constexpr auto const kWorkers = 100U;

  auto values = std::vector<std::int16_t>{};
  values.resize(kNValues);
  std::generate(begin(values), end(values), [&]() {
    return static_cast<std::int16_t>(get_random_number());
  });

  auto min = std::vector<std::int16_t>{};
  min.resize(1000, std::numeric_limits<std::int16_t>::max());

  auto workers = std::vector<std::thread>(kWorkers);
  auto next = std::atomic_size_t{0U};
  auto start = std::atomic_bool{false};
  for (auto i = 0U; i != kWorkers; ++i) {
    workers[i] = std::thread{[&]() {
      while (!start) {
        // wait for synchronized start
      }

      while (true) {
        auto idx = next++;
        if (idx >= values.size()) {
          return;
        }
        for (auto j = 0U; j != min.size(); ++j) {
          cista::fetch_min(min[j], values[idx]);
        }
      }
    }};
  }

  start.store(true);

  for (auto& w : workers) {
    w.join();
  }

  auto const smallest = *std::min_element(begin(values), end(values));
  for (auto const& x : min) {
    EXPECT_EQ(x, smallest);
  }
}
