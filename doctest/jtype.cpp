#include "json.h"
#include <doctest/doctest.h>

TEST_CASE("JsonTypeDetectionTest - EmptyString")
{
	CHECK_EQ(json::jtype::detect(""), json::jtype::not_valid);
}

TEST_CASE("JsonTypeDetectionTest - String")
{
	CHECK_EQ(json::jtype::detect(" \"test string\""), json::jtype::jstring);
}

TEST_CASE("JsonTypeDetectionTest - Number")
{
	CHECK_EQ(json::jtype::detect(" 123"), json::jtype::jnumber);
	CHECK_EQ(json::jtype::detect(" -123"), json::jtype::jnumber);
}

TEST_CASE("JsonTypeDetectionTest - Object")
{
	CHECK_EQ(json::jtype::detect(" {\"hello\":\"world\""), json::jtype::jobject);
}

TEST_CASE("JsonTypeDetectionTest - Array")
{
	CHECK_EQ(json::jtype::detect(" [1,2,3]"), json::jtype::jarray);
}

TEST_CASE("JsonTypeDetectionTest - Boolean")
{
	CHECK_EQ(json::jtype::detect(" true"), json::jtype::jbool);
	CHECK_EQ(json::jtype::detect(" false"), json::jtype::jbool);
}

TEST_CASE("JsonTypeDetectionTest - Null")
{
	CHECK_EQ(json::jtype::detect(" null"), json::jtype::jnull);
}

TEST_CASE("JsonTypeDetectionTest - Invalid")
{
	CHECK_EQ(json::jtype::detect(" abc"), json::jtype::not_valid);
}
