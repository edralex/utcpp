#include "include/lazycsv.hpp"

#include <catch2/catch_test_macros.hpp>

template<class T>
void check_rows(const T& parser, const std::vector<std::vector<std::string>>& expected_rows)
{
    auto row_index = 0;
    for (const auto row : parser)
    {
        auto expected_cells = expected_rows.at(row_index++);
        auto cell_index = 0;
        for (const auto cell : row)
        {
            REQUIRE(expected_cells.at(cell_index++) == cell.trimed());
        }
        REQUIRE(cell_index == expected_cells.size());
    }
    REQUIRE(row_index == expected_rows.size());
}

TEST_CASE("MmapSource zero length", "[mmap_source]")
{
    lazycsv::mmap_source source{ "inputs/zero_length.csv" };
    REQUIRE(source.size() == 0);
    REQUIRE(source.data() == nullptr);
}

TEST_CASE("MmapSource non-existent file", "[mmap_source]")
{
    REQUIRE_THROWS_AS(lazycsv::mmap_source{ "inputs/non_existent.csv" }, std::exception);
}

TEST_CASE("MmapSource basic CSV", "[mmap_source]")
{
    lazycsv::mmap_source source{ "inputs/basic.csv" };
    REQUIRE(source.size() == 47);
}

TEST_CASE("Parser basic CSV with mmap source and cells function", "[parser]")
{
    lazycsv::parser parser{ "inputs/basic.csv" };

    std::vector<std::vector<std::string>> expected_rows{
        { "A0", "B0", "C0", "D0" }, { "A1", "B1", "C1", "D1" }, { "A2", "B2", "C2", "D2" }, { "A3", "B3", "C3", "D3" }
    };

    const auto [a, b, c, d] = parser.header().cells(0, 1, 2, 3);
    auto header_cells = expected_rows.at(0);
    REQUIRE(header_cells[0] == a.raw());
    REQUIRE(header_cells[1] == b.raw());
    REQUIRE(header_cells[2] == c.raw());
    REQUIRE(header_cells[3] == d.raw());

    auto row_index = 1;
    for (const auto row : parser)
    {
        auto expected_cells = expected_rows.at(row_index++);
        const auto [a, b, c, d] = row.cells(0, 1, 2, 3);
        REQUIRE(expected_cells[0] == a.raw());
        REQUIRE(expected_cells[1] == b.raw());
        REQUIRE(expected_cells[2] == c.raw());
        REQUIRE(expected_cells[3] == d.raw());
    }
    REQUIRE(row_index == expected_rows.size());
}

TEST_CASE("Parser empty cell", "[parser]")
{
    lazycsv::parser<std::string, lazycsv::has_header<false>> parser{ ",B0,C0,D0\nA1,,,D1\nA2,B2,C2,\n,,,\n" };
    check_rows(parser, { { "", "B0", "C0", "D0" }, { "A1", "", "", "D1" }, { "A2", "B2", "C2", "" }, { "", "", "", "" } });
}

TEST_CASE("Parser empty rows", "[parser]")
{
    lazycsv::parser<std::string, lazycsv::has_header<false>> parser{ "\nA0,B0,C0,D0\nA1,B1,C1,D1\n\n\nA2,B2,C2,D2\n\nA3,B3,C3,D3\n\n" };
    check_rows(
        parser,
        { { "" },
          { "A0", "B0", "C0", "D0" },
          { "A1", "B1", "C1", "D1" },
          { "" },
          { "" },
          { "A2", "B2", "C2", "D2" },
          { "" },
          { "A3", "B3", "C3", "D3" },
          { "" } });
}

TEST_CASE("Parser skip last newline", "[parser]")
{
    lazycsv::parser<std::string, lazycsv::has_header<false>> parser{ "A0\nA1\nA2\nA3\n" };
    check_rows(parser, { { "A0" }, { "A1" }, { "A2" }, { "A3" } });
}

TEST_CASE("Parser without last newline", "[parser]")
{
    lazycsv::parser<std::string, lazycsv::has_header<false>> parser{ "A0\nA1\nA2\nA3" };
    check_rows(parser, { { "A0" }, { "A1" }, { "A2" }, { "A3" } });
}

TEST_CASE("Parser rows with different cell count", "[parser]")
{
    lazycsv::parser<std::string, lazycsv::has_header<false>> parser{ "A0,B0,C0\nA1,B1,C1,D1\nA2\n\n" };
    check_rows(parser, { { "A0", "B0", "C0" }, { "A1", "B1", "C1", "D1" }, { "A2" }, { "" } });
}

TEST_CASE("Parser trim cells with tab and space characters", "[parser]")
{
    lazycsv::parser<std::string, lazycsv::has_header<false>> parser{
        "A0, \t  B0,C0,D0\n A1,\tB1,C1,D1\t\n\tA2,B2 , C2 ,D2\n   A3,B3\t,\tC3,D3 \n"
    };
    check_rows(parser, { { "A0", "B0", "C0", "D0" }, { "A1", "B1", "C1", "D1" }, { "A2", "B2", "C2", "D2" }, { "A3", "B3", "C3", "D3" } });
}

TEST_CASE("Parser custom delimiter", "[parser]")
{
    lazycsv::parser<std::string, lazycsv::has_header<false>, lazycsv::delimiter<'-'>> parser{
        "A0-B0-C0-D0\nA1-B1-C1-D1\nA2-B2-C2-D2\nA3-B3-C3-D3\n"
    };
    check_rows(parser, { { "A0", "B0", "C0", "D0" }, { "A1", "B1", "C1", "D1" }, { "A2", "B2", "C2", "D2" }, { "A3", "B3", "C3", "D3" } });
}

TEST_CASE("Parser quoted cells", "[parser]")
{
    lazycsv::parser<std::string, lazycsv::has_header<false>> parser{
        "\"A0\"\"\",B0,C0,\n,\"B1,\",\"C1\",D1\n\"\",\",\",,\"D\"\"2\"\nA3, \"B3\" ,C3,\"\"\n"
    };
    check_rows(
        parser, { { "A0\"\"", "B0", "C0", "" }, { "", "B1,", "C1", "D1" }, { "", ",", "", "D\"\"2" }, { "A3", "\"B3\"", "C3", "" } });

    auto row_0 = parser.begin();
    auto [a0, b0, c0, d0] = row_0->cells(0, 1, 2, 3);
    REQUIRE("A0\"" == a0.unescaped());
    REQUIRE("B0" == b0.unescaped());
    REQUIRE("C0" == c0.unescaped());
    REQUIRE("" == d0.unescaped());

    auto row_2 = std::next(row_0, 2);
    auto [d2] = row_2->cells(3);
    REQUIRE("D\"2" == d2.unescaped());

    auto row_3 = std::next(row_0, 3);
    auto [b3] = row_3->cells(1);
    REQUIRE("\"B3\"" == b3.unescaped());
}
