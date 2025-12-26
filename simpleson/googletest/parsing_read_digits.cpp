#include "json.h"
#include <gtest/gtest.h>
#include <string>

TEST(JsonParsingTest, ReadDigitsNumericOnly)
{
	const char *input = " 123457890";
	ASSERT_STREQ(json::parsing::read_digits(input).c_str(), "123457890");
}

TEST(JsonParsingTest, ReadDigitsWithTrailingCharacter)
{
	const char *input = " 123457890a";
	ASSERT_STREQ(json::parsing::read_digits(input).c_str(), "123457890");
}
