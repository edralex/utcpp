#include "json.h"
#include <doctest/doctest.h>
#include <string>
#include <cstring>

TEST_CASE("JsonStringEncodingTest - AlphanumericCharacters")
{
	const char *input = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string result = json::parsing::encode_string(input);
	const char *expected_result = "\"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\"";
	CHECK_EQ(strcmp(result.c_str(), expected_result), 0);

	std::string echo = json::parsing::decode_string(result.c_str());
	CHECK_EQ(strcmp(echo.c_str(), input), 0);
}

TEST_CASE("JsonStringEncodingTest - EscapeCharacters")
{
	const char *input = "\" \\ / \b \f \n \r \t";
	std::string result = json::parsing::encode_string(input);
	const char *expected_result = "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"";
	CHECK_EQ(strcmp(result.c_str(), expected_result), 0);

	std::string echo = json::parsing::decode_string(result.c_str());
	CHECK_EQ(strcmp(echo.c_str(), input), 0);
}
