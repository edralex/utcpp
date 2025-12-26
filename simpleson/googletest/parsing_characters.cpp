#include "json.h"
#include <gtest/gtest.h>
#include <string>

TEST(JsonStringEncodingTest, AlphanumericCharacters)
{
	const char *input = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string result = json::parsing::encode_string(input);
	const char *expected_result = "\"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\"";
	ASSERT_STREQ(result.c_str(), expected_result);

	std::string echo = json::parsing::decode_string(result.c_str());
	ASSERT_STREQ(echo.c_str(), input);
}

TEST(JsonStringEncodingTest, EscapeCharacters)
{
	const char *input = "\" \\ / \b \f \n \r \t";
	std::string result = json::parsing::encode_string(input);
	const char *expected_result = "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"";
	ASSERT_STREQ(result.c_str(), expected_result);

	std::string echo = json::parsing::decode_string(result.c_str());
	ASSERT_STREQ(echo.c_str(), input);
}
