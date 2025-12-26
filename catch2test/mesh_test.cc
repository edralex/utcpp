#include <catch2/catch_test_macros.hpp>
#include <cstdio>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/mmap.h"
#include "cista/serialization.h"
#endif

using namespace cista;
namespace data = cista::offset;

struct vec3 {
  float x, y, z;
};

struct mat4 {
  data::array<float, 16> d;
};

struct Vertex {
  vec3 position;
  vec3 normal;
};

struct Mesh {
  data::vector<mat4> transforms;
  data::vector<Vertex> vertices;
};
using Meshes = data::vector<Mesh>;

struct Staging {
  int stepIndex;
  data::vector<float> distances;
  data::vector<vec3> translations;
  data::vector<vec3> rotations;
};
using Stagings = data::vector<Staging>;

struct Group {
  Meshes meshes;
  Stagings stagings;
};

TEST_CASE("mesh test", "[serialization][mmap]") {
  constexpr auto const FILENAME = "group.bin";

  std::remove(FILENAME);

  {
    Group g;
    Mesh m;
    m.transforms.emplace_back(mat4{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f,
                                   0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f});
    g.meshes.emplace_back(m);

    cista::buf<cista::mmap> mmap{cista::mmap{FILENAME}};
    cista::serialize(mmap, g);
  }  // EOL graph

  auto b = cista::file(FILENAME, "r").content();
  auto const g = cista::deserialize<Group>(b);
  REQUIRE(g != nullptr);
  CHECK(g->meshes[0].transforms[0].d[0] == 0.0f);
  CHECK(g->meshes[0].transforms[0].d[1] == 1.0f);
  CHECK(g->meshes[0].transforms[0].d[2] == 2.0f);
  CHECK(g->meshes[0].transforms[0].d[3] == 3.0f);
  CHECK(g->meshes[0].transforms[0].d[4] == 4.0f);
  CHECK(g->meshes[0].transforms[0].d[5] == 5.0f);
  CHECK(g->meshes[0].transforms[0].d[6] == 6.0f);
  CHECK(g->meshes[0].transforms[0].d[7] == 7.0f);
  CHECK(g->meshes[0].transforms[0].d[8] == 0.0f);
  CHECK(g->meshes[0].transforms[0].d[9] == 1.0f);
  CHECK(g->meshes[0].transforms[0].d[10] == 2.0f);
  CHECK(g->meshes[0].transforms[0].d[11] == 3.0f);
  CHECK(g->meshes[0].transforms[0].d[12] == 4.0f);
  CHECK(g->meshes[0].transforms[0].d[13] == 5.0f);
  CHECK(g->meshes[0].transforms[0].d[14] == 6.0f);
  CHECK(g->meshes[0].transforms[0].d[15] == 7.0f);
}