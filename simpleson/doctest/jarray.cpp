#include "json.h"
#include <doctest/doctest.h>
#include <math.h>
#include <cstring>

TEST_CASE("JsonParserTest - ParseArrayWithMixedTypes")
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
	CHECK(result.is_array());

	// Test string representations of array elements
	CHECK_EQ(strcmp(result.get(0).c_str(), "123.456"), 0);
	CHECK_EQ(strcmp(result.get(1).c_str(), "\"hello \\\" world\""), 0);
	CHECK_EQ(strcmp(result.get(2).c_str(), "[1,2,3]"), 0);
	CHECK_EQ(strcmp(result.get(3).c_str(), "true"), 0);
	CHECK_EQ(strcmp(result.get(4).c_str(), "null"), 0);
	CHECK_EQ(strcmp(result.get(5).c_str(), "[{\"key\":\"value\"}]"), 0);
	CHECK_EQ(strcmp(result.get(6).c_str(), "[\"hello\",\"world\"]"), 0);
	CHECK_EQ(strcmp(result.get(7).c_str(), "[]"), 0);

	// Test type checking
	CHECK(result.array(3).is_true());
	CHECK(result.array(4).is_null());
	CHECK(result.array(5).array(0).as_object() == json::jobject::parse("{\"key\":\"value\"}"));

	// Test serialization
	CHECK_EQ(strcmp(result.as_string().c_str(), input), 0);

	// Test copy constructor
	json::jobject copy(result);
	CHECK_EQ(strcmp(copy.as_string().c_str(), result.as_string().c_str()), 0);
}
