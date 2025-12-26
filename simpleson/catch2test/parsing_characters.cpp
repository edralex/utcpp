#include "json.h"
#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("JsonStringEncoding alphanumeric characters", "[json][encoding]") {
	const char *input = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string result = json::parsing::encode_string(input);
	const char *expected_result = "\"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\"";
	REQUIRE(result == std::string(expected_result));

	std::string echo = json::parsing::decode_string(result.c_str());
	REQUIRE(echo == std::string(input));
}

TEST_CASE("JsonStringEncoding escape characters", "[json][encoding]") {
	const char *input = "\" \\ / \b \f \n \r \t";
	std::string result = json::parsing::encode_string(input);
	const char *expected_result = "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"";
	REQUIRE(result == std::string(expected_result));

	std::string echo = json::parsing::decode_string(result.c_str());
	REQUIRE(echo == std::string(input));
}
