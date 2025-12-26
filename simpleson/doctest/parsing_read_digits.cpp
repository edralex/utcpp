#include "json.h"
#include <doctest/doctest.h>
#include <string>
#include <cstring>

TEST_CASE("JsonParsingTest - ReadDigitsNumericOnly")
{
	const char *input = " 123457890";
	CHECK_EQ(strcmp(json::parsing::read_digits(input).c_str(), "123457890"), 0);
}

TEST_CASE("JsonParsingTest - ReadDigitsWithTrailingCharacter")
{
	const char *input = " 123457890a";
	CHECK_EQ(strcmp(json::parsing::read_digits(input).c_str(), "123457890"), 0);
}
