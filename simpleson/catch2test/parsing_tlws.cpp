#include "json.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("JsonParsing trim leading whitespace with content", "[json][parsing]") {
	const char *test_string = " \t \n \v \f \r abc123";
	REQUIRE(std::string(json::parsing::tlws(test_string)) == "abc123");

	// Test idempotency - calling again should give same result
	json::parsing::tlws(test_string);
	REQUIRE(std::string(json::parsing::tlws(test_string)) == "abc123");
}

TEST_CASE("JsonParsing trim leading whitespace only whitespace", "[json][parsing]") {
	const char *test_string = " \t \n \v \f \r";
	REQUIRE(std::string(json::parsing::tlws(test_string)) == "");
}
