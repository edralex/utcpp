#include <gtest/gtest.h>  // Замена doctest.h
#include <queue>
#include <set>
#include <cstdio>  // для std::remove

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/serialization.h"
#endif

// ... (Весь код определения графа, узлов, ребер и функции bfs остается без изменений)
namespace data = cista::raw;

constexpr auto const CHECKSUM_INTEGRITY_AND_VERSION =
    sizeof(void*) == 4 ? 13555853665375988614ULL : 10074751584048131577ULL;

namespace graphns::raw {

struct node;

using node_id_t = uint32_t;

struct edge {
  data::ptr<node> from_;
  data::ptr<node> to_;
};

struct node {
  void add_edge(edge* e) { edges_.emplace_back(e); }
  node_id_t id() const { return id_; }

  node_id_t id_{0};
  node_id_t fill_{0};
  data::vector<data::ptr<edge>> edges_;
  data::string name_;
};

struct graph {
  node* make_node(data::string name) {
    return nodes_
        .emplace_back(data::make_unique<node>(
            node{next_node_id_++, 0, data::vector<data::ptr<edge>>(0u),
                 std::move(name)}))
        .get();
  }

  edge* make_edge(node_id_t const from, node_id_t const to) {
    return edges_
        .emplace_back(
            data::make_unique<edge>(edge{nodes_[from].get(), nodes_[to].get()}))
        .get();
  }

  data::vector<data::unique_ptr<node>> nodes_;
  data::vector<data::unique_ptr<edge>> edges_;
  node_id_t next_node_id_{0};
  node_id_t fill_{0};
};

}  // namespace graphns::raw

using namespace graphns::raw;

inline std::set<node const*> bfs(node const* entry) {
  std::queue<node const*> q;
  std::set<node const*> visited;

  q.emplace(entry);

  while (!q.empty()) {
    auto const next = q.front();
    q.pop();

    if (visited.count(next) != 0) {
      continue;
    }

    visited.emplace(next);

    for (auto const& e : next->edges_) {
      q.emplace(e->to_);
    }
  }

  return visited;
}
// Конец неизмененной части

// Тест-кейс, преобразованный из TEST_CASE("graph raw serialize file")
TEST(GraphRawSerialize, File) {
  constexpr auto const FILENAME = "raw_graph.bin";
  constexpr auto const MODE =
      cista::mode::WITH_INTEGRITY | cista::mode::WITH_VERSION;

  std::remove(FILENAME);

  {
    graph g;

    auto const n1 = g.make_node(data::string{"NODE A"});
    auto const n2 = g.make_node(data::string{"NODE B"});
    auto const n3 = g.make_node(data::string{"NODE C"});

    auto const e1 = g.make_edge(n1->id(), n2->id());
    auto const e2 = g.make_edge(n2->id(), n3->id());
    auto const e3 = g.make_edge(n3->id(), n1->id());

    n1->add_edge(e1);
    n2->add_edge(e2);
    n3->add_edge(e3);

    cista::file f{FILENAME, "w+"};
    cista::serialize<MODE>(f, g);

    // Проверка CHECK заменена на EXPECT_EQ
    EXPECT_EQ(f.checksum(), CHECKSUM_INTEGRITY_AND_VERSION);
  }

  auto b = cista::file(FILENAME, "r").content();
  EXPECT_EQ(cista::hash(b), CHECKSUM_INTEGRITY_AND_VERSION);

  auto const g = cista::deserialize<graph, MODE>(b);
  auto const visited = bfs(g->nodes_[0].get());

  auto it = visited.begin();
  ASSERT_NE(it, visited.end());
  EXPECT_EQ((*it++)->name_, data::string{"NODE A"});

  ASSERT_NE(it, visited.end());
  EXPECT_EQ((*it++)->name_, data::string{"NODE B"});

  ASSERT_NE(it, visited.end());
  EXPECT_EQ((*it++)->name_, data::string{"NODE C"});

  EXPECT_EQ(it, visited.end());
}

// Тест-кейс, преобразованный из TEST_CASE("graph raw serialize buf")
TEST(GraphRawSerialize, Buf) {
  constexpr auto const MODE =
      cista::mode::WITH_INTEGRITY | cista::mode::WITH_VERSION;

  cista::byte_buf buf;
  {
    graph g;

    auto const n1 = g.make_node(data::string{"NODE A"});
    auto const n2 = g.make_node(data::string{"NODE B"});
    auto const n3 = g.make_node(data::string{"NODE C"});

    auto const e1 = g.make_edge(n1->id(), n2->id());
    auto const e2 = g.make_edge(n2->id(), n3->id());
    auto const e3 = g.make_edge(n3->id(), n1->id());

    n1->add_edge(e1);
    n2->add_edge(e2);
    n3->add_edge(e3);

    cista::buf b;
    cista::serialize<MODE>(b, g);

    EXPECT_EQ(b.checksum(), CHECKSUM_INTEGRITY_AND_VERSION);

    buf = std::move(b.buf_);
  }

  EXPECT_EQ(cista::hash(buf), CHECKSUM_INTEGRITY_AND_VERSION);

  auto const g = cista::deserialize<graph, MODE>(buf);
  auto const visited = bfs(g->nodes_[0].get());

  auto it = visited.begin();
  ASSERT_NE(it, visited.end());
  EXPECT_EQ((*it++)->name_, data::string{"NODE A"});

  ASSERT_NE(it, visited.end());
  EXPECT_EQ((*it++)->name_, data::string{"NODE B"});

  ASSERT_NE(it, visited.end());
  EXPECT_EQ((*it++)->name_, data::string{"NODE C"});

  EXPECT_EQ(it, visited.end());
}

// Стандартная точка входа для Google Test
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
