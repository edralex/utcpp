#include "json.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("JsonObject compound assignment operator", "[json]") {
	json::jobject obj1;
	obj1["key1"] = "value1";
	json::jobject obj2;
	obj2["key2"] = "value2";

	obj1 += obj2;

	REQUIRE(obj1.size() == 2);
	REQUIRE(obj1.has_key("key1"));
	REQUIRE(obj1["key1"].as_string() == "value1");
	REQUIRE(obj1.has_key("key2"));
	REQUIRE(obj1["key2"].as_string() == "value2");
}

TEST_CASE("JsonObject addition operator", "[json]") {
	json::jobject obj1;
	obj1["key1"] = "value1";
	json::jobject obj2;
	obj2["key2"] = "value2";
	obj1 += obj2;

	json::jobject obj3;
	obj3["key3"] = "value3";

	json::jobject obj4 = obj1 + obj3;
	REQUIRE(obj4.size() == 3);
	REQUIRE(obj4.has_key("key1"));
	REQUIRE(obj4["key1"].as_string() == "value1");
	REQUIRE(obj4.has_key("key2"));
	REQUIRE(obj4["key2"].as_string() == "value2");
	REQUIRE(obj4.has_key("key3"));
	REQUIRE(obj4["key3"].as_string() == "value3");
}
