#include "json.h"
#include <gtest/gtest.h>
#include <math.h>

TEST(JsonParserTest, ParseArrayWithMixedTypes)
{
	const char *input =
	   "["
		  "123.456,"
		  "\"hello \\\" world\","
		  "[1,2,3],"
		  "true,"
		  "null,"
		  "[{\"key\":\"value\"}],"
		  "[\"hello\",\"world\"],"
		  "[]"
	   "]";

	json::jobject result = json::jobject::parse(input);

	// Test array type
	ASSERT_TRUE(result.is_array());

	// Test string representations of array elements
	ASSERT_STREQ(result.get(0).c_str(), "123.456");
	ASSERT_STREQ(result.get(1).c_str(), "\"hello \\\" world\"");
	ASSERT_STREQ(result.get(2).c_str(), "[1,2,3]");
	ASSERT_STREQ(result.get(3).c_str(), "true");
	ASSERT_STREQ(result.get(4).c_str(), "null");
	ASSERT_STREQ(result.get(5).c_str(), "[{\"key\":\"value\"}]");
	ASSERT_STREQ(result.get(6).c_str(), "[\"hello\",\"world\"]");
	ASSERT_STREQ(result.get(7).c_str(), "[]");

	// Test type checking
	ASSERT_TRUE(result.array(3).is_true());
	ASSERT_TRUE(result.array(4).is_null());
	ASSERT_TRUE(result.array(5).array(0).as_object() == json::jobject::parse("{\"key\":\"value\"}"));

	// Test serialization
	ASSERT_STREQ(result.as_string().c_str(), input);

	// Test copy constructor
	json::jobject copy(result);
	ASSERT_STREQ(copy.as_string().c_str(), result.as_string().c_str());
}
