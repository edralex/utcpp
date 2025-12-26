#include "json.h"
#include <doctest/doctest.h>

TEST_CASE("JsonObjectTest - CompoundAssignmentOperator")
{
	json::jobject obj1;
	obj1["key1"] = "value1";
	json::jobject obj2;
	obj2["key2"] = "value2";

	obj1 += obj2;

	CHECK_EQ(obj1.size(), 2);
	CHECK(obj1.has_key("key1"));
	CHECK_EQ(obj1["key1"].as_string(), "value1");
	CHECK(obj1.has_key("key2"));
	CHECK_EQ(obj1["key2"].as_string(), "value2");
}

TEST_CASE("JsonObjectTest - AdditionOperator")
{
	json::jobject obj1;
	obj1["key1"] = "value1";
	json::jobject obj2;
	obj2["key2"] = "value2";
	obj1 += obj2;

	json::jobject obj3;
	obj3["key3"] = "value3";

	json::jobject obj4 = obj1 + obj3;
	CHECK_EQ(obj4.size(), 3);
	CHECK(obj4.has_key("key1"));
	CHECK_EQ(obj4["key1"].as_string(), "value1");
	CHECK(obj4.has_key("key2"));
	CHECK_EQ(obj4["key2"].as_string(), "value2");
	CHECK(obj4.has_key("key3"));
	CHECK_EQ(obj4["key3"].as_string(), "value3");
}
