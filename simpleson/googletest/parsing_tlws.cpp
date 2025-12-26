#include "json.h"
#include <gtest/gtest.h>

TEST(JsonParsingTest, TrimLeadingWhitespaceWithContent)
{
	const char *test_string = " \t \n \v \f \r abc123";
	ASSERT_STREQ(json::parsing::tlws(test_string), "abc123");

	// Test idempotency - calling again should give same result
	json::parsing::tlws(test_string);
	ASSERT_STREQ(json::parsing::tlws(test_string), "abc123");
}

TEST(JsonParsingTest, TrimLeadingWhitespaceOnlyWhitespace)
{
	const char *test_string = " \t \n \v \f \r";
	ASSERT_STREQ(json::parsing::tlws(test_string), "");
}
