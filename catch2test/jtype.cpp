#include "json.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("JsonTypeDetection empty string", "[json][type]") {
	REQUIRE(json::jtype::detect("") == json::jtype::not_valid);
}

TEST_CASE("JsonTypeDetection string", "[json][type]") {
	REQUIRE(json::jtype::detect(" \"test string\"") == json::jtype::jstring);
}

TEST_CASE("JsonTypeDetection number", "[json][type]") {
	REQUIRE(json::jtype::detect(" 123") == json::jtype::jnumber);
	REQUIRE(json::jtype::detect(" -123") == json::jtype::jnumber);
}

TEST_CASE("JsonTypeDetection object", "[json][type]") {
	REQUIRE(json::jtype::detect(" {\"hello\":\"world\"") == json::jtype::jobject);
}

TEST_CASE("JsonTypeDetection array", "[json][type]") {
	REQUIRE(json::jtype::detect(" [1,2,3]") == json::jtype::jarray);
}

TEST_CASE("JsonTypeDetection boolean", "[json][type]") {
	REQUIRE(json::jtype::detect(" true") == json::jtype::jbool);
	REQUIRE(json::jtype::detect(" false") == json::jtype::jbool);
}

TEST_CASE("JsonTypeDetection null", "[json][type]") {
	REQUIRE(json::jtype::detect(" null") == json::jtype::jnull);
}

TEST_CASE("JsonTypeDetection invalid", "[json][type]") {
	REQUIRE(json::jtype::detect(" abc") == json::jtype::not_valid);
}
