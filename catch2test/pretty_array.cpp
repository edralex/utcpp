#include "json.h"
#include <catch2/catch_test_macros.hpp>
#include <math.h>

TEST_CASE("JsonPrettyPrint parse and format array", "[json][pretty]") {
	const char *input =
	   "[\n"
		   "\t123.456,\n"
		   "\t\"hello \\\" world\",\n"
		   "\t[\n"
				"\t\t1,\n"
				"\t\t2,\n"
				"\t\t3\n"
			"\t],\n"
		  "\ttrue,\n"
		  "\tnull,\n"
		  "\t[\n"
				"\t\t{\n"
					"\t\t\t\"key\": \"value\"\n"
				"\t\t}\n"
			"\t],\n"
		  "\t[\n"
				"\t\t\"hello\",\n"
				"\t\t\"world\"\n"
			"\t],\n"
		  "\t[]\n"
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

	// Test pretty print serialization
	REQUIRE(result.pretty() == std::string(input));

	// Test copy constructor
	json::jobject copy(result);
	REQUIRE(copy.as_string() == result.as_string());
}
