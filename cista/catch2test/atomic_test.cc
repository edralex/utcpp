#include <catch2/catch_test_macros.hpp>

#include <thread>
#include <vector>
#include <algorithm>
#include <limits>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/atomic.h"
#endif

// Объявляем функцию (реализация предполагается где-то в другом файле или в рамках тестов)
unsigned long get_random_number();

TEST_CASE("atomic min test", "[atomic]") {
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
      while (!start.load(std::memory_order_acquire)) {
        // ждём синхронизированный старт
      }

      while (true) {
        auto idx = next.fetch_add(1, std::memory_order_relaxed);
        if (idx >= values.size()) {
          return;
        }
        for (auto j = 0U; j != min.size(); ++j) {
          cista::fetch_min(min[j], values[idx]);
        }
      }
    }};
  }

  start.store(true, std::memory_order_release);

  for (auto& w : workers) {
    w.join();
  }

  auto const smallest = *std::min_element(begin(values), end(values));
  for (auto const& x : min) {
    CHECK(x == smallest);
  }
}