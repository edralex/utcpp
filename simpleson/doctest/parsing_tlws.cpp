#include "json.h"
#include <doctest/doctest.h>
#include <cstring>

TEST_CASE("JsonParsingTest - TrimLeadingWhitespaceWithContent")
{
	const char *test_string = " \t \n \v \f \r abc123";
	CHECK_EQ(strcmp(json::parsing::tlws(test_string), "abc123"), 0);

	// Test idempotency - calling again should give same result
	json::parsing::tlws(test_string);
	CHECK_EQ(strcmp(json::parsing::tlws(test_string), "abc123"), 0);
}

TEST_CASE("JsonParsingTest - TrimLeadingWhitespaceOnlyWhitespace")
{
	const char *test_string = " \t \n \v \f \r";
	CHECK_EQ(strcmp(json::parsing::tlws(test_string), ""), 0);
}
