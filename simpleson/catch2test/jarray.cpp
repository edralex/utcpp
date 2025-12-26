#include "json.h"
#include <catch2/catch_test_macros.hpp>
#include <math.h>

TEST_CASE("JsonParser parses array with mixed types", "[json]") {
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
	REQUIRE(result.is_array());

	// Test string representations of array elements
	REQUIRE(std::string(result.get(0).c_str()) == "123.456");
	REQUIRE(std::string(result.get(1).c_str()) == "\"hello \\\" world\"");
	REQUIRE(std::string(result.get(2).c_str()) == "[1,2,3]");
	REQUIRE(std::string(result.get(3).c_str()) == "true");
	REQUIRE(std::string(result.get(4).c_str()) == "null");
	REQUIRE(std::string(result.get(5).c_str()) == "[{\"key\":\"value\"}]");
	REQUIRE(std::string(result.get(6).c_str()) == "[\"hello\",\"world\"]");
	REQUIRE(std::string(result.get(7).c_str()) == "[]");

	// Test type checking
	REQUIRE(result.array(3).is_true());
	REQUIRE(result.array(4).is_null());
	REQUIRE(result.array(5).array(0).as_object() == json::jobject::parse("{\"key\":\"value\"}"));

	// Test serialization
	REQUIRE(result.as_string() == std::string(input));

	// Test copy constructor
	json::jobject copy(result);
	REQUIRE(copy.as_string() == result.as_string());
}
