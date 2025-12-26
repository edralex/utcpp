#include "json.h"
#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("JsonParsing read digits numeric only", "[json][parsing]") {
	const char *input = " 123457890";
	REQUIRE(json::parsing::read_digits(input) == "123457890");
}

TEST_CASE("JsonParsing read digits with trailing character", "[json][parsing]") {
	const char *input = " 123457890a";
	REQUIRE(json::parsing::read_digits(input) == "123457890");
}
