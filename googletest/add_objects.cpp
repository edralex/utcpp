#include "json.h"
#include <gtest/gtest.h>

TEST(JsonObjectTest, CompoundAssignmentOperator)
{
	json::jobject obj1;
	obj1["key1"] = "value1";
	json::jobject obj2;
	obj2["key2"] = "value2";

	obj1 += obj2;

	ASSERT_EQ(obj1.size(), 2);
	ASSERT_TRUE(obj1.has_key("key1"));
	ASSERT_STREQ(obj1["key1"].as_string().c_str(), "value1");
	ASSERT_TRUE(obj1.has_key("key2"));
	ASSERT_STREQ(obj1["key2"].as_string().c_str(), "value2");
}

TEST(JsonObjectTest, AdditionOperator)
{
	json::jobject obj1;
	obj1["key1"] = "value1";
	json::jobject obj2;
	obj2["key2"] = "value2";
	obj1 += obj2;

	json::jobject obj3;
	obj3["key3"] = "value3";

	json::jobject obj4 = obj1 + obj3;
	ASSERT_EQ(obj4.size(), 3);
	ASSERT_TRUE(obj4.has_key("key1"));
	ASSERT_STREQ(obj4["key1"].as_string().c_str(), "value1");
	ASSERT_TRUE(obj4.has_key("key2"));
	ASSERT_STREQ(obj4["key2"].as_string().c_str(), "value2");
	ASSERT_TRUE(obj4.has_key("key3"));
	ASSERT_STREQ(obj4["key3"].as_string().c_str(), "value3");
}
